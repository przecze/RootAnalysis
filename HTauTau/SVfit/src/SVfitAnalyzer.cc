#include <sstream>
#include <bitset>

#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "TF1.h"
#include "Math/LorentzVector.h"
#include "Math/WrappedTF1.h"
#include "Math/RootFinder.h"
#include "Math/Boost.h"
#include "Math/Rotation3D.h"
#include "Math/AxisAngle.h"

#include "SVfitAnalyzer.h"
#include "SVfitHistograms.h"
#include "MuTauSpecifics.h"
#include "TauTauSpecifics.h"
#include "Tools.h"
#include "MLObjectMessenger.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SVfitAnalyzer::SVfitAnalyzer(const std::string & aName, const std::string & aDecayMode) : HTTAnalyzer(aName, aDecayMode), aCovMET(2,2){

#pragma omp critical
  {  
    SVFitAlgo.addLogM_fixed(true, 4.0);
    SVFitAlgo.setLikelihoodFileName("");
    SVFitAlgo.setMaxObjFunctionCalls(100000);
    SVFitAlgo.setVerbosity(1);
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SVfitAnalyzer::~SVfitAnalyzer(){

  if(myHistos_) delete myHistos_;
  if(myChannelSpecifics) delete myChannelSpecifics;

  myHistos_ = 0;
  myChannelSpecifics = 0;  
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SVfitAnalyzer::initialize(TDirectory* aDir,
			       pat::strbitset *aSelections){

  mySelections_ = aSelections;

  myHistos_ = new SVfitHistograms(aDir, selectionFlavours_, myChannelSpecifics->getDecayModeName());
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SVfitAnalyzer::setAnalysisObjects(const EventProxyHTT & myEventProxy){

  HTTAnalyzer::setAnalysisObjects(myEventProxy);

  aCovMET[0][0] = aPair.getMETMatrix().at(0);
  aCovMET[0][1] = aPair.getMETMatrix().at(1);
  aCovMET[1][0] = aPair.getMETMatrix().at(2);
  aCovMET[1][1] = aPair.getMETMatrix().at(3);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SVfitAnalyzer::finalize(){

  myHistos_->finalizeHistograms(myChannelSpecifics->getCategoryRejester());
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
TLorentzVector SVfitAnalyzer::computeMTT(const std::string & algoName){

  //Legs
  double mass1;
  int decay1 = -1;
  classic_svFit::MeasuredTauLepton::kDecayType type1;
  if(std::abs(aLeg1.getPDGid())==11) {
    mass1 = 0.51100e-3; //electron mass
    type1 = classic_svFit::MeasuredTauLepton::kTauToElecDecay;
  }
  else if(std::abs(aLeg1.getPDGid())==13) {
    mass1 = 0.10566; //muon mass
    type1 = classic_svFit::MeasuredTauLepton::kTauToMuDecay;
  }
  else{//tau->hadrs.
    decay1 = aLeg1.getProperty(PropertyEnum::decayMode);
    mass1 = aLeg1.getP4().M();
    if(decay1==0)
      mass1 = 0.13957; //pi+/- mass
    type1 = classic_svFit::MeasuredTauLepton::kTauToHadDecay;
  }

  double mass2;
  int decay2 = -1;
  classic_svFit::MeasuredTauLepton::kDecayType type2;
  if(std::abs(aLeg2.getPDGid())==11) {
    mass2 = 0.51100e-3; //electron mass
    type2 = classic_svFit::MeasuredTauLepton::kTauToElecDecay;
  }
  else if(std::abs(aLeg2.getPDGid())==13) {
    mass2 = 0.10566; //muon mass
    type2 = classic_svFit::MeasuredTauLepton::kTauToMuDecay;
  }
  else{//tau->hadrs.
    decay2 = aLeg2.getProperty(PropertyEnum::decayMode);
    mass2 = aLeg2.getP4().M();
    if(decay2==0) mass2 = 0.13957; //pi+/- mass
    type2 = classic_svFit::MeasuredTauLepton::kTauToHadDecay;
  }
 
  //Leptons for SVFit
  TVector3 recPV = aEvent.getRefittedPV();
  TVector3 recSV = aLeg1.getSV();  
  double cosGJReco = (recSV - recPV).Unit()*aLeg1.getP4().Vect().Unit();
  classic_svFit::MeasuredTauLepton aLepton1(type1, aLeg1.getP4().Pt(), aLeg1.getP4().Eta(),
					    aLeg1.getP4().Phi(), mass1, decay1);

  TVector3 genPV = aEvent.getGenPV();
  TVector3 genSV = aGenLeg1.getSV();
  TLorentzVector aP4 = aGenLeg1.getChargedP4();  
  double ip3D = aLeg1.getPCA().Mag();
 
  aLepton1.setCosGJ(cosGJReco);
  aLepton1.setIP3D(ip3D);

  classic_svFit::MeasuredTauLepton aLepton2(type2, aLeg2.getP4().Pt(), aLeg2.getP4().Eta(),
					    aLeg2.getP4().Phi(), mass2, decay2);
  recSV = aLeg2.getSV();  
  cosGJReco = (recSV - recPV).Unit()*aLeg2.getP4().Vect().Unit();
  ip3D = aLeg2.getPCA().Mag();

  genSV = aGenLeg2.getSV();
  aP4 = aGenLeg2.getChargedP4();
   
  aLepton2.setCosGJ(cosGJReco);
  aLepton2.setIP3D(ip3D);

  std::vector<classic_svFit::MeasuredTauLepton> measuredTauLeptons;
  measuredTauLeptons.push_back(aLepton1);
  measuredTauLeptons.push_back(aLepton2);
  
  if(aCovMET[0][0]==0 && aCovMET[1][0]==0 && aCovMET[0][1]==0 && aCovMET[1][1]==0) return TLorentzVector(); //singular covariance matrix

  TLorentzVector aResult;
  if(algoName=="SVfit") aResult = runSVFitAlgo(measuredTauLeptons, aMET, aCovMET);
  if(algoName=="fastMTT") aResult = runFastMTTAlgo(measuredTauLeptons, aMET, aCovMET);
  
  return aResult;
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
TLorentzVector SVfitAnalyzer::runFastMTTAlgo(const std::vector<classic_svFit::MeasuredTauLepton> & measuredTauLeptons,
                                           const HTTParticle &aMET, const TMatrixD &covMET){

  fastMTTAlgo.run(measuredTauLeptons, aMET.getP4().X(), aMET.getP4().Y(), covMET);
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > aP4 = fastMTTAlgo.getBestP4();
  
  TLorentzVector p4SVFit(aP4.X(), aP4.Y(), aP4.Z(), aP4.E());

  aP4 = fastMTTAlgo.getTau1P4();
  SVFitLeg1P4 = TLorentzVector(aP4.X(), aP4.Y(), aP4.Z(), aP4.E());
  
  aP4 = fastMTTAlgo.getTau2P4();
  SVFitLeg2P4 = TLorentzVector(aP4.X(), aP4.Y(), aP4.Z(), aP4.E());
  
  return p4SVFit;
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
TLorentzVector SVfitAnalyzer::runSVFitAlgo(const std::vector<classic_svFit::MeasuredTauLepton> & measuredTauLeptons,
                                           const HTTParticle &aMET, const TMatrixD &covMET){

  SVFitAlgo.setVerbosity(0);

  SVFitAlgo.addLogM_fixed(true, 4.0);
  if(myChannelSpecifics->getDecayModeName()=="MuTau") SVFitAlgo.addLogM_fixed(true, 4.0);
  else if(myChannelSpecifics->getDecayModeName()=="TauTau") SVFitAlgo.addLogM_fixed(true, 5.0);
  else if(myChannelSpecifics->getDecayModeName()=="MuMu") SVFitAlgo.addLogM_fixed(true, 3.0);
  
  SVFitAlgo.setMaxObjFunctionCalls(100000);
  //SVFitAlgo.setLikelihoodFileName("testClassicSVfit.root");
  //SVFitAlgo.setTreeFileName("markovChainTree.root");
  TVector3 recPV = aEvent.getRefittedPV();
  TVector3 SVLeg2 = aLeg2.getSV();
  SVLeg2 -=recPV;

  TVector3 SVLeg1 = aGenLeg1.getSV();
  SVLeg1 -=recPV;

  SVFitAlgo.integrate(measuredTauLeptons, aMET.getP4().X(), aMET.getP4().Y(), covMET);
  classic_svFit::DiTauSystemHistogramAdapter* diTauAdapter = static_cast<classic_svFit::DiTauSystemHistogramAdapter*>(SVFitAlgo.getHistogramAdapter());
  float mcMass = diTauAdapter->getMass();

  TLorentzVector p4SVFit;
  if(SVFitAlgo.isValidSolution() )
    {//Get solution
      p4SVFit.SetPtEtaPhiM(diTauAdapter->getPt(),
			   diTauAdapter->getEta(),
			   diTauAdapter->getPhi(),
			   mcMass);
    }
  return p4SVFit;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
double SVfitAnalyzer::runCAAlgo(const HTTParticle & aLeg1, const HTTParticle & aLeg2,
				const HTTParticle & aMET){

  TMatrixD A(2,2);
  A[0][0] = sin(aLeg1.getChargedP4().Theta())*
    cos(aLeg1.getChargedP4().Phi());
  
  A[1][1] = sin(aLeg2.getChargedP4().Theta())*
    sin(aLeg2.getChargedP4().Phi());
  
  A[0][1] = sin(aLeg2.getChargedP4().Theta())*
    cos(aLeg2.getChargedP4().Phi());

  A[1][0] = sin(aLeg1.getChargedP4().Theta())*
    sin(aLeg1.getChargedP4().Phi());

  TMatrixD invA = A.Invert();

  double e1 = invA[0][0]*aMET.getP4().Px() +
    invA[0][1]*aMET.getP4().Py();

  double e2 = invA[1][0]*aMET.getP4().Px() +
    invA[1][1]*aMET.getP4().Py();
  
  double x1 =  aLeg1.getChargedP4().E()/(aLeg1.getP4().E() + e1);
  double x2 =  aLeg2.getChargedP4().E()/(aLeg2.getP4().E() + e2);

  if(x1<0 || x2<0) return 0.0;
  double caMass = (aLeg1.getChargedP4() + aLeg2.getChargedP4()).M()/std::sqrt(x1*x2);

  return caMass;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
std::tuple<double, double> SVfitAnalyzer::getTauMomentum(const TLorentzVector & visP4, double cosGJ){

  double mTau = 1.77685;
  double mTau2 = std::pow(mTau,2);
  
  double mVis =  visP4.M();
  double mVis2 = std::pow(mVis,2);
  double pVis =  visP4.P();
  double pVis2 = std::pow(pVis,2);
  double cosGJ2 = std::pow(cosGJ,2);
  double sinGJ2 = 1.0 - cosGJ2;

  double b2 = (mVis2 + mTau2)*pVis*cosGJ;

  double delta = (mVis2 + pVis2)*(std::pow(mVis2 - mTau2, 2) - 4.0*mTau2*pVis2*sinGJ2); 
  if(delta<0){   
    return std::tuple<double, double>(0, 0);
  }

  double twoA = 2.0*(mVis2 + pVis2*sinGJ2);
  double solution1 = (b2 - sqrt(delta))/twoA;
  double solution2 = (b2 + sqrt(delta))/twoA;

  double tauEnergy1 = sqrt(pow(solution1,2) + mTau2);
  double tauEnergy2 = sqrt(pow(solution2,2) + mTau2);

  solution1 = tauEnergy1;
  solution2 = tauEnergy2;

  return std::tuple<double, double>(solution1, solution2);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SVfitAnalyzer::fillControlHistos(const std::string & hNameSuffix){

  const TLorentzVector & aVisSum = aLeg1.getP4() + aLeg2.getP4();
  TLorentzVector tautauGen = aGenLeg1.getP4() + aGenLeg2.getP4();
  TLorentzVector nunuGen = tautauGen - 
    aGenLeg1.getChargedP4() - aGenLeg2.getChargedP4() -
    aGenLeg1.getNeutralP4() - aGenLeg2.getNeutralP4();
  TLorentzVector SVFitP4 = computeMTT("fastMTT");  
  float visMass = aVisSum.M();
  double delta = (SVFitP4.M() - tautauGen.M())/tautauGen.M();

  double massCA = runCAAlgo(aLeg1, aLeg2, aMET);

  double recoX1 = 0.0, recoX2 = 0.0;
  std::tie(recoX1, recoX2) = fastMTTAlgo.getBestX();

  myHistos_->fill1DHistogram("h1DMassVis"+hNameSuffix, visMass);
  myHistos_->fill1DHistogram("h1DMassGen"+hNameSuffix,tautauGen.M());
  myHistos_->fill1DHistogram("h1DMassFastMTT"+hNameSuffix,SVFitP4.M());
  myHistos_->fill1DHistogram("h1DMassCA"+hNameSuffix, massCA);
  myHistos_->fill1DHistogram("h1DDeltaFastMTT"+hNameSuffix,delta);

  delta = (massCA - tautauGen.M())/tautauGen.M();
  myHistos_->fill1DHistogram("h1DDeltaCA"+hNameSuffix,delta);
  myHistos_->fill1DHistogram("h1DCpuTimeFast"+hNameSuffix,fastMTTAlgo.getCpuTime("scan"));

  delta = (aMET.getP4().X() - nunuGen.X())/nunuGen.X();
  myHistos_->fill1DHistogram("h1DDeltaMET_X_Res"+hNameSuffix, delta);

  delta = (aMET.getP4().Y() - nunuGen.Y())/nunuGen.Y();
  myHistos_->fill1DHistogram("h1DDeltaMET_Y_Res"+hNameSuffix, delta);

  myHistos_->fill2DHistogram("h2DDeltaMET_X_Res_Vs_Mass"+hNameSuffix, nunuGen.Perp(), delta);

  
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool SVfitAnalyzer::analyze(const EventProxyBase& iEvent, ObjectMessenger *aMessenger){

  const EventProxyHTT & myEventProxy = static_cast<const EventProxyHTT&>(iEvent);
  sampleName = HTTAnalysis::getSampleName(myEventProxy);

  std::string hNameSuffix = sampleName;

  if(!myEventProxy.pairs->size()) return true;
  setAnalysisObjects(myEventProxy);

  bool isGoodReco = aGenLeg1.getP4().DeltaR(aLeg1.getP4())<0.4 &&
		    aGenLeg2.getP4().DeltaR(aLeg2.getP4())<0.4;
  
  isGoodReco |= aGenLeg2.getP4().DeltaR(aLeg1.getP4())<0.4 &&
		aGenLeg1.getP4().DeltaR(aLeg2.getP4())<0.4;
  
  bool goodGenTau = aGenLeg1.getP4().E()>1.0 && aGenLeg2.getP4().E()>1.0;							   

  isGoodReco = aGenLeg2.getP4().DeltaR(aLeg2.getP4())<0.4;
  goodGenTau = aGenLeg2.getP4().E()>1.0;

  if(sampleName=="WAllJets"){
    goodGenTau = true;
    isGoodReco = true;
  }
  
  if(sampleName=="DYAllJetsMatchL"){
    goodGenTau = true;
    isGoodReco = true;
  }
  
  if(isGoodReco && goodGenTau){
    fillControlHistos(hNameSuffix);
  
    if(aMessenger and std::string("MLObjectMessenger").compare((aMessenger->name()).substr(0,17))==0 ) // if NULL it will do nothing
      {
	// Putting data to MLObjectMessenger
	try
	  {
	    MLObjectMessenger* mess = (MLObjectMessenger*)aMessenger;
	    mess->putObjectVector(const_cast <const HTTParticle*>(&aLeg1), "legs");
	    mess->putObjectVector(const_cast <const HTTParticle*>(&aLeg2), "legs");
	    mess->putObjectVector(const_cast <const HTTParticle*>(&aMET), "jets");
	    mess->putObjectVector(const_cast <const HTTParticle*>(&aJet1), "jets");
	    mess->putObjectVector(const_cast <const HTTParticle*>(&aJet2), "jets");
	    mess->putObject(&aCovMET[0][0], "covMET00");
	    mess->putObject(&aCovMET[0][1], "covMET01");
	    mess->putObject(&aCovMET[1][0], "covMET10");
	    mess->putObject(&aCovMET[1][1], "covMET11");

	    double genMass = (aGenLeg1.getP4() + aGenLeg2.getP4()).M();      
	    mess->putObject(&genMass, "genMass");

	    double visMass = (aLeg1.getP4() + aLeg2.getP4()).M();      
	    mess->putObject(&visMass, "visMass");

	    double caMass = runCAAlgo(aLeg1, aLeg2, aMET);
	    mess->putObject(&caMass, "caMass");

	    TLorentzVector fastMTTP4 = computeMTT("fastMTT");
	    double fastMTTMass = fastMTTP4.M();
	    mess->putObject(&fastMTTMass, "fastMTTMass");
	  }
	catch(const std::exception& e)
	  {
	    std::throw_with_nested(std::runtime_error("[ERROR] UNKNOWN ERROR IN "+std::string( __func__ )+ " WHEN PUTTING DATA TO MLObjectMessenger!"));
	  }                    
      }
  }
  else return false;

  return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

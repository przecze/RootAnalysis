#include <sstream>
#include <bitset>

#include "HTTAnalyzer.h"
#include "HTTHistograms.h"
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
HTTAnalyzer::HTTAnalyzer(const std::string & aName):Analyzer(aName){

  ///Load ROOT file with PU histograms.
  std::string filePath = "Data_Pileup_2015D_Feb02.root";
  puDataFile_ = new TFile(filePath.c_str());

  filePath = "MC_Spring15_PU25_Startup.root";
  puMCFile_ = new TFile(filePath.c_str());

  ntupleFile_ = 0;
  
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
HTTAnalyzer::~HTTAnalyzer(){

  if(myHistos_) delete myHistos_;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Analyzer* HTTAnalyzer::clone() const{

  HTTAnalyzer* clone = new HTTAnalyzer(name());
  clone->setHistos(myHistos_);
  return clone;

};
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HTTAnalyzer::initialize(TDirectory* aDir,
			     pat::strbitset *aSelections){

  mySelections_ = aSelections;
  
  myHistos_ = new HTTHistograms(aDir, selectionFlavours_);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HTTAnalyzer::finalize(){ 

  myHistos_->finalizeHistograms(0,1.0);
 
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HTTAnalyzer::getPreselectionEff(const EventProxyHTT & myEventProxy){

    TH1F *hStatsFromFile = (TH1F*)myEventProxy.getTTree()->GetCurrentFile()->Get("hStats");

    std::string hName = "h1DStats"+getSampleName(myEventProxy);
    TH1F *hStats = myHistos_->get1DHistogram(hName.c_str(),true);

    float genWeight = getGenWeight(myEventProxy);

    hStats->SetBinContent(2,hStatsFromFile->GetBinContent(hStatsFromFile->FindBin(1))*genWeight);   
    hStats->SetBinContent(3,hStatsFromFile->GetBinContent(hStatsFromFile->FindBin(3))*genWeight);
    
    delete hStatsFromFile;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
std::string HTTAnalyzer::getSampleName(const EventProxyHTT & myEventProxy){

  if(myEventProxy.event->getSampleType()==0) return "Data";
  if(myEventProxy.event->getSampleType()==1){
    int decayModeBoson = myEventProxy.event->getDecayModeBoson();
    std::string decayName;
    if(decayModeBoson==7) decayName = "MuMu";
    else if(decayModeBoson==6) decayName = "EE";
    else if(decayModeBoson==0) decayName = "MuTau";
    else decayName = "Other";

    std::string fileName = myEventProxy.getTTree()->GetCurrentFile()->GetName();
    std::string jetsName = "";    
    if(fileName.find("DY1JetsToLL")!=std::string::npos) jetsName ="DY1Jets";
    else if(fileName.find("DY2JetsToLL")!=std::string::npos) jetsName = "DY2Jets";
    else if(fileName.find("DY3JetsToLL")!=std::string::npos) jetsName = "DY3Jets";
    else if(fileName.find("DY4JetsToLL")!=std::string::npos) jetsName = "DY4Jets";
    else jetsName = "DYJets";   
    return jetsName+decayName;    
  }
  if(myEventProxy.event->getSampleType()==11) return "DYJetsLowM";
  if(myEventProxy.event->getSampleType()==2 || myEventProxy.event->getSampleType()==-1){ ///BUG in sample number for nJets
    std::string fileName = myEventProxy.getTTree()->GetCurrentFile()->GetName();
    if(fileName.find("WJetsToLNu")!=std::string::npos && myEventProxy.event->getLHEnOutPartons()==0) return "W0Jets";
    else if(fileName.find("WJetsToLNu")!=std::string::npos && myEventProxy.event->getLHEnOutPartons()>0) return "WAllJets";
    else if(fileName.find("W1JetsToLNu")!=std::string::npos) return "W1Jets";
    else if(fileName.find("W2JetsToLNu")!=std::string::npos) return "W2Jets";
    else if(fileName.find("W3JetsToLNu")!=std::string::npos) return "W3Jets";
    else if(fileName.find("W4JetsToLNu")!=std::string::npos) return "W4Jets";
    else if(fileName.find("W5JetsToLNu")!=std::string::npos) return "W5Jets";
  }
  if(myEventProxy.event->getSampleType()==3) return "TTbar";
  if(myEventProxy.event->getSampleType()==4) return "H";
  if(myEventProxy.event->getSampleType()==5) return "A";

  return "Unknown";
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
float HTTAnalyzer::getPUWeight(const EventProxyHTT & myEventProxy){

  ///RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1 MINIAOD
  ///is produced with PU profile matching the Run2015 data. No nee to PU rescale.
  return 1.0;

  ///Load histogram only once,later fetch it from vector<TH1F*>
  ///At the same time divide the histogram to get the weight.
  ///First load Data PU
  if(!hPUVec_.size())  hPUVec_.resize(64);

  if(!hPUVec_[myEventProxy.event->getSampleType()]){
    std::string hName = "pileup";
    TH1F *hPUData = (TH1F*)puDataFile_->Get(hName.c_str());
    TH1F *hPUSample = (TH1F*)puMCFile_->Get(hName.c_str());
    ///Normalise both histograms.
    hPUData->Scale(1.0/hPUData->Integral(0,hPUData->GetNbinsX()+1));
    hPUSample->Scale(1.0/hPUSample->Integral(0,hPUSample->GetNbinsX()+1));
    ///
    hPUData->SetDirectory(0);
    hPUSample->SetDirectory(0);
    hPUData->Divide(hPUSample);
    hPUData->SetName(("h1DPUWeight"+getSampleName(myEventProxy)).c_str());
    ///To get uniform treatment put weight=1.0 for under/overlow bins of
    ///data PU, as nPU for data has a dummy value.
    if(getSampleName(myEventProxy)=="Data"){
      hPUData->SetBinContent(0,1.0);
      hPUData->SetBinContent(hPUData->GetNbinsX()+1,1.0);
    }
    hPUVec_[myEventProxy.event->getSampleType()] =  hPUData;
  }

  int iBinPU = hPUVec_[myEventProxy.event->getSampleType()]->FindBin(myEventProxy.event->getNPU());
  
  return  hPUVec_[myEventProxy.event->getSampleType()]->GetBinContent(iBinPU);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
float HTTAnalyzer::getGenWeight(const EventProxyHTT & myEventProxy){

  if(myEventProxy.event->getSampleType()==0) return 1.0;
  //if(myEventProxy.event->getSampleType()==1) return myEventProxy->getMCWeight()/23443.423;  
  //if(myEventProxy.event->getSampleType()==2) return myEventProxy->getMCWeight()/225892.45;  
  //if(myEventProxy.event->getSampleType()==3) return myEventProxy->getMCWeight()/6383;

  return 1;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HTTAnalyzer::fillControlHistos(const std::string & hNameSuffix, float eventWeight){

  ///Fill histograms with number of PV.
  myHistos_->fill1DHistogram("h1DNPV"+hNameSuffix,aEvent.getNPV(),eventWeight);

  ///Fill SVfit and visible masses
  myHistos_->fill1DHistogram("h1DMassSV"+hNameSuffix,aPair.getP4SVFit().M(),eventWeight);
  myHistos_->fill1DHistogram("h1DMassVis"+hNameSuffix,aPair.getP4().M(),eventWeight);
  
  ///Fill muon
  myHistos_->fill1DHistogram("h1DMassTrans"+hNameSuffix,aPair.getMTMuon(),eventWeight);
  myHistos_->fill1DHistogram("h1DPtMuon"+hNameSuffix,aMuon.getP4().Pt(),eventWeight);
  myHistos_->fill1DHistogram("h1DEtaMuon"+hNameSuffix,aMuon.getP4().Eta(),eventWeight);
  myHistos_->fill1DHistogram("h1DPhiMuon"+hNameSuffix,aMuon.getP4().Phi(),eventWeight);
  myHistos_->fill1DHistogram("h1DIsoMuon"+hNameSuffix,aMuon.getProperty(PropertyEnum::combreliso),eventWeight);
  myHistos_->fill1DHistogram("h1DnPCAMuon"+hNameSuffix,aMuon.getPCARefitPV().Mag(),eventWeight);

  ///Fill tau
  myHistos_->fill1DHistogram("h1DPtTau"+hNameSuffix,aTau.getP4().Pt(),eventWeight);
  myHistos_->fill1DHistogram("h1DEtaTau"+hNameSuffix,aTau.getP4().Eta(),eventWeight);
  myHistos_->fill1DHistogram("h1DPhiTau"+hNameSuffix,aTau.getP4().Phi() ,eventWeight);
  myHistos_->fill1DHistogram("h1DIDTau"+hNameSuffix,aTau.getProperty(PropertyEnum::byCombinedIsolationDeltaBetaCorrRaw3Hits) ,eventWeight);  
  myHistos_->fill1DHistogram("h1DStatsDecayMode"+hNameSuffix, aTau.getProperty(PropertyEnum::decayMode), eventWeight);
  myHistos_->fill1DHistogram("h1DnPCATau"+hNameSuffix,aTau.getPCARefitPV().Mag(),eventWeight);

  ///Fill leading tau track pt
  myHistos_->fill1DHistogram("h1DPtTauLeadingTk"+hNameSuffix,aTau.getProperty(PropertyEnum::leadChargedParticlePt),eventWeight);
  ///Fill jets info           
  myHistos_->fill1DHistogram("h1DStatsNJets30"+hNameSuffix,nJets30,eventWeight);
  myHistos_->fill1DHistogram("h1DPtLeadingJet"+hNameSuffix,aJet.getP4().Pt(),eventWeight);
  myHistos_->fill1DHistogram("h1DEtaLeadingJet"+hNameSuffix,aJet.getP4().Eta(),eventWeight);
  myHistos_->fill1DHistogram("h1DCSVBtagLeadingJet"+hNameSuffix,aJet.getProperty(PropertyEnum::bCSVscore),eventWeight);
  
  myHistos_->fill1DHistogram("h1DPtMET"+hNameSuffix,aPair.getMET().Mod(),eventWeight);

  fillDecayPlaneAngle(hNameSuffix, eventWeight);

  if(aJet.getProperty(PropertyEnum::bDiscriminator)>0.8){
    myHistos_->fill1DHistogram("h1DPtLeadingBJet"+hNameSuffix,aJet.getP4().Pt(),eventWeight);
    myHistos_->fill1DHistogram("h1DEtaLeadingBJet"+hNameSuffix,aJet.getP4().Eta(),eventWeight);
  }   
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool HTTAnalyzer::fillVertices(const std::string & sysType){
  
  TVector3 aVertexGen = aEvent.getGenPV();
  TVector3 aVertex;
  if(sysType.find("AODPV")!=std::string::npos) aVertex = aEvent.getAODPV();
  if(sysType.find("RefitPV")!=std::string::npos) aVertex = aEvent.getRefittedPV();
  
  float pullX = aVertexGen.X() - aVertex.X();
  float pullY = aVertexGen.Y() - aVertex.Y();
  float pullZ = aVertexGen.Z() - aVertex.Z();

  myHistos_->fill1DHistogram("h1DVxPullX_"+sysType,pullX);
  myHistos_->fill1DHistogram("h1DVxPullY_"+sysType,pullY);
  myHistos_->fill1DHistogram("h1DVxPullZ_"+sysType,pullZ);

  myHistos_->fill2DHistogram("h2DVxPullVsNTrackTrans_"+sysType, aEvent.getNTracksInRefit(), sqrt(pullX*pullX + pullY*pullY));
  myHistos_->fill2DHistogram("h2DVxPullVsNTrackLong_"+sysType, aEvent.getNTracksInRefit(), pullZ);
  
  return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HTTAnalyzer::fillDecayPlaneAngle(const std::string & hNameSuffix, float eventWeight){
  
  ///Method from http://arxiv.org/abs/1108.0670 (S. Berge)
  ///take impact parameters instead of tau momentum.
  ///calculate angles in pi+ - pi- rest frame
  std::pair<float,float>  angles;

  ///Method from http://arxiv.org/abs/hep-ph/0204292 (Was)
  ///Angle between rho decay planes in the rho-rho.
  ///Here we take rho on one side, mu on the other
  std::pair<float,float>  anglesIPRho;  

  TLorentzVector muonTk = aMuon.getChargedP4();
  TLorentzVector tauLeadingTk = aTau.getChargedP4();
  TLorentzVector muonPCA, tauPCA;

  fillVertices(hNameSuffix);

  if(hNameSuffix.find("AODPV")!=std::string::npos){
    muonPCA = TLorentzVector(aMuon.getPCA(),0);
    tauPCA = TLorentzVector(aTau.getPCA(),0);
  }    
  if(hNameSuffix.find("RefitPV")!=std::string::npos){
    muonPCA = TLorentzVector(aMuon.getPCARefitPV(),0);
    tauPCA = TLorentzVector(aTau.getPCARefitPV(),0);
  }
  if(hNameSuffix.find("GenPV")!=std::string::npos){
    muonPCA = TLorentzVector(aMuon.getPCAGenPV(),0);
    tauPCA = TLorentzVector(aTau.getPCAGenPV(),0);
  }

  if(aMuon.getCharge()>0) {
    angles = angleBetweenPlanes(muonTk, muonPCA, tauLeadingTk, tauPCA);
    anglesIPRho = angleBetweenPlanes(muonTk, muonPCA, tauLeadingTk, aTau.getNeutralP4());				     
  }
  else{
    angles = angleBetweenPlanes(tauLeadingTk, tauPCA, muonTk, muonPCA);
    anglesIPRho = angleBetweenPlanes(tauLeadingTk, aTau.getNeutralP4(), muonTk, muonPCA);
  }

  myHistos_->fillProfile("hProfRecoVsMagGen_"+hNameSuffix,
			 aGenHadTau.getPCA().Mag(),
			 tauPCA.Vect().Mag(),
			 eventWeight);
 
  ///////////////////////////////////////////////////////////
  float yTau =  2.*tauLeadingTk.Pt()/aTau.getP4().Pt() - 1.;
  float shiftedIPrho = anglesIPRho.first +(yTau<0)*(1-2*(anglesIPRho.first>M_PI))*M_PI;
 
  if(aTau.getProperty(PropertyEnum::decayMode)!=tauDecay1ChargedPion0PiZero && isOneProng(aTau.getProperty(PropertyEnum::decayMode))){
     myHistos_->fill1DHistogram("h1DyTau"+hNameSuffix,yTau,eventWeight);
     myHistos_->fill1DHistogram("h1DPhi_nVecIP_"+hNameSuffix,shiftedIPrho,eventWeight);
     if(yTau>0){
       myHistos_->fill1DHistogram("h1DPhi_nVecIP_yTauPos"+hNameSuffix,anglesIPRho.first,eventWeight);
     }
     else{
       myHistos_->fill1DHistogram("h1DPhi_nVecIP_yTauNeg"+hNameSuffix,anglesIPRho.first,eventWeight);
     }
  }

  if(aTau.getProperty(PropertyEnum::decayMode)==tauDecay1ChargedPion0PiZero){
    float cosPhiNN =  tauPCA.Vect().Unit().Dot(muonPCA.Vect().Unit());

    myHistos_->fill1DHistogram("h1DPhi_nVectors"+hNameSuffix,angles.first,eventWeight);
    myHistos_->fill1DHistogram("h1DCosPhiNN_"+hNameSuffix,cosPhiNN);
    
    float cosMuon =  muonPCA.Vect().Unit()*aGenMuonTau.getPCA().Unit();
    float cosTau = tauPCA.Vect().Unit()*aGenHadTau.getPCA().Unit();
    
    myHistos_->fillProfile("hProfPhiVsMag_"+hNameSuffix,aGenMuonTau.getPCA().Mag(),cosMuon);
    myHistos_->fillProfile("hProfPhiVsMag_"+hNameSuffix,aGenHadTau.getPCA().Mag(),cosTau);       
    }  
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HTTAnalyzer::fillGenDecayPlaneAngle(const std::string & hNameSuffix, float eventWeight){
  
  ///Method from http://arxiv.org/abs/1108.0670 (Berger)
  ///take impact parameters instead of tau momentum.
  ///calculate angles in pi+ - pi- rest frame  
  std::pair<float,float>  angles;

  //Method from http://arxiv.org/abs/hep-ph/0204292 (Was)
  //Angle between rho decay planes in the rho-rho.
  ///Here we take rho on one side, mu on the other
  std::pair<float,float>  anglesIPRho;

  TLorentzVector muonTk = aGenMuonTau.getChargedP4();
  TLorentzVector tauLeadingTk = aGenHadTau.getChargedP4();
  TLorentzVector muonPCA(aGenMuonTau.getPCA(),0);
  TLorentzVector tauPCA(aGenHadTau.getPCA(),0);

  if(aMuon.getCharge()>0) {
    angles = angleBetweenPlanes(muonTk, muonPCA, tauLeadingTk, tauPCA);
    anglesIPRho = angleBetweenPlanes(muonTk, muonPCA, tauLeadingTk, aTau.getNeutralP4());				     
  }
  else{
    angles = angleBetweenPlanes(tauLeadingTk, tauPCA, muonTk, muonPCA);
    anglesIPRho = angleBetweenPlanes(tauLeadingTk, aTau.getNeutralP4(), muonTk, muonPCA);
  }

  if(aGenHadTau.getProperty(PropertyEnum::decayMode)==tauDecay1ChargedPion0PiZero){
  
    float cosPhiNN =  muonPCA.Vect().Unit().Dot(tauPCA.Vect().Unit());
    myHistos_->fill1DHistogram("h1DCosPhiNN_"+hNameSuffix,cosPhiNN);
    myHistos_->fill1DHistogram("h1DPhi_nVectors"+hNameSuffix,angles.first,eventWeight);
  }
  //Method from http://arxiv.org/abs/hep-ph/0204292 (Was)
  //Angle between rho decay planes in the rho-rho.
  ///Here we take rho on one side, mu on the other
  float yTau =  2.*tauLeadingTk.Pt()/(aGenHadTau.getChargedP4()+aGenHadTau.getNeutralP4()).Pt() - 1.;
  float shiftedIPrho = anglesIPRho.first +(yTau<0)*(1-2*(anglesIPRho.first>M_PI))*M_PI;
  
  if(aGenHadTau.getProperty(PropertyEnum::decayMode)!=tauDecay1ChargedPion0PiZero && isOneProng(aGenHadTau.getProperty(PropertyEnum::decayMode))){  
    myHistos_->fill1DHistogram("h1DyTau"+hNameSuffix,yTau,eventWeight);
    myHistos_->fill1DHistogram("h1DPhi_nVecIP_"+hNameSuffix,shiftedIPrho,eventWeight);
    if(yTau>0) myHistos_->fill1DHistogram("h1DPhi_nVecIP_yTauPos"+hNameSuffix,anglesIPRho.first,eventWeight);
    else myHistos_->fill1DHistogram("h1DPhi_nVecIP_yTauNeg"+hNameSuffix,anglesIPRho.first,eventWeight);
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
std::pair<float,float> HTTAnalyzer::angleBetweenPlanes(const TLorentzVector &tau1, 
						       const TLorentzVector &tau1Daughter,
						       const TLorentzVector &tau2, 
						       const TLorentzVector &tau2Daughter,
						       bool sgn){
  //Boost all 4v to (tau1+tau2) rest frame
  TVector3 boost = (tau1+tau2).BoostVector();

  TLorentzVector tau1Star = tau1;
  TLorentzVector tau2Star = tau2;
  tau1Star.Boost(-boost);
  tau2Star.Boost(-boost);
  
  TLorentzVector tau1DaughterStar = tau1Daughter;
  tau1DaughterStar.Boost(-boost);
  
  TLorentzVector tau2DaughterStar = tau2Daughter;
  tau2DaughterStar.Boost(-boost);

  //define common direction and normal vectors to decay planes
  TVector3 direction = tau1Star.Vect().Unit();
  TVector3 n1 = ( direction.Cross( tau1DaughterStar.Vect() ) ).Unit(); 
  TVector3 n2 = ( direction.Cross( tau2DaughterStar.Vect() ) ).Unit(); 

  ///angle between decay planes
  float phi=TMath::ACos(n1*n2);

  if(sgn){
    ///phase
    float calO = direction * ( n1.Cross(n2) );
    if(calO<0)
      phi = 2*TMath::Pi() - phi;
  }

  ///angle between tau1 and tau2 daughter momentums
  float rho=TMath::ACos( (tau1DaughterStar.Vect().Unit() )*(tau2DaughterStar.Vect().Unit() ) );

  return std::make_pair(phi,rho);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
std::vector<std::string> HTTAnalyzer::getTauDecayName(int decModeMinus, int decModePlus){

  std::vector<std::string> types;

  if(decModeMinus==tauDecay1ChargedPion0PiZero && decModePlus==tauDecay1ChargedPion0PiZero) types.push_back("PiPi0Pi0");

  if(isOneProng(decModeMinus) && isOneProng(decModePlus) ) types.push_back("1Prong1Prong");

  if( (decModeMinus==tauDecay1ChargedPion0PiZero && isLepton(decModePlus) ) ||
      (isLepton(decModeMinus) && decModePlus==tauDecay1ChargedPion0PiZero)) types.push_back("Lepton1Prong0Pi0");
    
  if( (isOneProng(decModeMinus) && isLepton(decModePlus) ) ||
      ( isLepton(decModeMinus) && isOneProng(decModePlus) ) ) types.push_back("Lepton1Prong");

  if(decModeMinus==tauDecay1ChargedPion1PiZero && decModePlus==tauDecay1ChargedPion1PiZero ) types.push_back("PiPlusPiMinus2Pi0");


  if( isOneProng(decModeMinus) && decModeMinus!=tauDecay1ChargedPion0PiZero && 
      isOneProng(decModePlus) && decModePlus!=tauDecay1ChargedPion0PiZero )   types.push_back("1Prong1ProngXPi0");

  if(isLepton(decModePlus) && isLepton(decModeMinus)) types.push_back("LeptonLepton");

  return types;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool HTTAnalyzer::isOneProng(int decMode){
  if(decMode==tauDecay1ChargedPion0PiZero ||
     decMode==tauDecay1ChargedPion1PiZero ||
     decMode==tauDecay1ChargedPion2PiZero ||
     decMode==tauDecay1ChargedPion3PiZero ) return true;
  else return false;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool HTTAnalyzer::isLepton(int decMode){
  if(decMode==tauDecaysElectron || decMode==tauDecayMuon) return true;
  else return false;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
std::vector<HTTParticle> HTTAnalyzer::getSeparatedJets(const EventProxyHTT & myEventProxy,
						float deltaR){

  std::vector<HTTParticle> separatedJets;
  
  for(auto aJet: *myEventProxy.jets){
    float dRTau = aJet.getP4().DeltaR(aTau.getP4());
    float dRMu = aMuon.getP4().DeltaR(aTau.getP4());
    if(dRTau>deltaR && dRMu>deltaR) separatedJets.push_back(aJet);
  }

  return separatedJets;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HTTAnalyzer::setAnalysisObjects(const EventProxyHTT & myEventProxy){

  aEvent = *myEventProxy.event;  
  aPair = (*myEventProxy.pairs)[0];
  aTau = aPair.getTau();
  aMuon = aPair.getMuon();

  if(myEventProxy.genLeptons && myEventProxy.genLeptons->size()){
    HTTParticle aGenTau =  myEventProxy.genLeptons->at(0);    
    if(aGenTau.getProperty(PropertyEnum::decayMode)==tauDecayMuon) aGenMuonTau = aGenTau;
    else if(aGenTau.getProperty(PropertyEnum::decayMode)!=tauDecaysElectron) aGenHadTau = aGenTau;
    if(myEventProxy.genLeptons->size()>1){
      HTTParticle aGenTau =  myEventProxy.genLeptons->at(1);
      if(aGenTau.getProperty(PropertyEnum::decayMode)==tauDecayMuon) aGenMuonTau = aGenTau;
      else if(aGenTau.getProperty(PropertyEnum::decayMode)!=tauDecaysElectron) aGenHadTau = aGenTau;
    }
  }
  
  aSeparatedJets = getSeparatedJets(myEventProxy, 0.5);
  aJet = aSeparatedJets.size() ? aSeparatedJets[0] : HTTParticle();
  nJets30 = count_if(aSeparatedJets.begin(), aSeparatedJets.end(),[](const HTTParticle & aJet){return aJet.getP4().Pt()>30;});  
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
std::pair<bool, bool> HTTAnalyzer::checkTauDecayMode(const EventProxyHTT & myEventProxy){

  bool goodGenDecayMode = false;
  bool goodRecoDecayMode = false;

  std::vector<std::string> decayNamesGen = getTauDecayName(aGenHadTau.getProperty(PropertyEnum::decayMode),
							   aGenMuonTau.getProperty(PropertyEnum::decayMode));
  std::vector<std::string> decayNamesReco = getTauDecayName(aTau.getProperty(PropertyEnum::decayMode),HTTAnalyzer::tauDecayMuon);

  for(auto it: decayNamesGen) if(it.find("Lepton1Prong")!=std::string::npos) goodGenDecayMode = true;
  for(auto it: decayNamesReco) if(it.find("Lepton1Prong")!=std::string::npos) goodRecoDecayMode = true;

  return std::pair<bool, bool>(goodGenDecayMode, goodRecoDecayMode);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HTTAnalyzer::addBranch(TTree *tree){/*tree->Branch("muonPt",&muonPt);*/}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool HTTAnalyzer::analyze(const EventProxyBase& iEvent){

  const EventProxyHTT & myEventProxy = static_cast<const EventProxyHTT&>(iEvent);

  std::string sampleName = getSampleName(myEventProxy);

  std::string hNameSuffix = sampleName;
  float puWeight = getPUWeight(myEventProxy);
  float genWeight = getGenWeight(myEventProxy);
  float eventWeight = puWeight*genWeight;

  //Fill bookkeeping histogram. Bin 1 holds sum of weights.
  myHistos_->fill1DHistogram("h1DStats"+sampleName,0,eventWeight);
  getPreselectionEff(myEventProxy);

  if(!myEventProxy.pairs->size()) return true;

  setAnalysisObjects(myEventProxy);

  std::pair<bool, bool> goodDecayModes = checkTauDecayMode(myEventProxy);
  bool goodGenDecayMode = goodDecayModes.first;
  bool goodRecoDecayMode = goodDecayModes.second;

  if(goodGenDecayMode) fillGenDecayPlaneAngle(sampleName+"GenNoOfflineSel", eventWeight);
  
  ///This stands for core selection, that is common to all regions.
  bool tauKinematics = aTau.getP4().Pt()>30 && fabs(aTau.getP4().Eta())<2.3;
  int tauIDmask = 0;
  
  for(unsigned int iBit=0;iBit<aEvent.ntauIds;iBit++){
    if(aEvent.tauIDStrings[iBit]=="byTightIsolationMVArun2v1DBoldDMwLT") tauIDmask |= (1<<iBit);
    if(aEvent.tauIDStrings[iBit]=="againstMuonTight3") tauIDmask |= (1<<iBit);
    if(aEvent.tauIDStrings[iBit]=="againstElectronVLooseMVA6") tauIDmask |= (1<<iBit);
  }

  bool tauID = ( (int)aTau.getProperty(PropertyEnum::tauID) & tauIDmask) == tauIDmask;
  
  bool muonKinematics = aMuon.getP4().Pt()>19 && fabs(aMuon.getP4().Eta())<2.1;
  bool trigger = aMuon.hasTriggerMatch(TriggerEnum::HLT_IsoMu18);
  if(sampleName=="Data") trigger = aMuon.hasTriggerMatch(TriggerEnum::HLT_IsoMu22) || aMuon.hasTriggerMatch(TriggerEnum::HLT_IsoMu18);
  bool cpMuonSelection = aMuon.getPCA().Mag()>0.005;    
  bool cpTauSelection = (aTau.getProperty(PropertyEnum::decayMode)==tauDecay1ChargedPion0PiZero && aTau.getPCA().Mag()>0.005) ||
                        (aTau.getProperty(PropertyEnum::decayMode)!=tauDecay1ChargedPion0PiZero &&
			 isOneProng(aTau.getProperty(PropertyEnum::decayMode))); 
  bool cpSelection = cpMuonSelection && cpTauSelection;
  
  if(!tauKinematics || !tauID || !muonKinematics || !trigger) return true;
 //if(!cpSelection) return true;

  ///Note: parts of the signal/control region selection are applied in the following code.
  ///FIXME AK: this should be made in a more clear way.
  bool SS = aTau.getCharge()*aMuon.getCharge() == 1;
  bool OS = aTau.getCharge()*aMuon.getCharge() == -1;
  bool baselineSelection = OS && aPair.getMTMuon()<40 && aMuon.getProperty(PropertyEnum::combreliso)<0.1;
  bool wSelection = aPair.getMTMuon()>60 && aMuon.getProperty(PropertyEnum::combreliso)<0.1;
  bool qcdSelectionSS = SS;
  bool qcdSelectionOS = OS;
  //bool ttSelection = aJet.getProperty(PropertyEnum::bDiscriminator)>0.5 && nJets30>1;
  bool ttSelection = nJets30>1;
  bool mumuSelection =  aPair.getMTMuon()<40 &&  aMuon.getProperty(PropertyEnum::combreliso)<0.1 && aPair.getP4().M()>85 && aPair.getP4().M()<95;

  ///Histograms for the baseline selection  
  if(baselineSelection){
    fillControlHistos(hNameSuffix, eventWeight);
    if(goodGenDecayMode || sampleName.find("WJets")!=std::string::npos){
      fillDecayPlaneAngle(hNameSuffix+"RefitPV", eventWeight);
      fillDecayPlaneAngle(hNameSuffix+"AODPV", eventWeight);
      fillDecayPlaneAngle(hNameSuffix+"GenPV", eventWeight);
      fillDecayPlaneAngle(hNameSuffix+"Gen", eventWeight);
    }
  }

  ///Histograms for the QCD control region
  if(qcdSelectionSS){
    hNameSuffix = sampleName+"qcdselSS";
    ///SS ans OS isolation histograms are filled only for mT<40 to remove possible contamination
    //from TT in high mT region.
    if(aPair.getMTMuon()<40) myHistos_->fill1DHistogram("h1DIso"+hNameSuffix,aMuon.getProperty(PropertyEnum::combreliso),eventWeight);
    ///Fill SS histos in signal mu isolation region. Those histograms
    ///provide shapes for QCD estimate in signal region and in various control regions.
    ///If control region has OS we still use SS QCD estimate.
    if(aPair.getMTMuon()<40 && aMuon.getProperty(PropertyEnum::combreliso)<0.1) fillControlHistos(hNameSuffix, eventWeight);
    if(aPair.getMTMuon()>60 && aMuon.getProperty(PropertyEnum::combreliso)<0.1){
      myHistos_->fill1DHistogram("h1DMassTrans"+hNameSuffix+"wselSS",aPair.getMTMuon(),eventWeight);    
      myHistos_->fill1DHistogram("h1DMassTrans"+hNameSuffix+"wselOS",aPair.getMTMuon(),eventWeight);
      fillDecayPlaneAngle(hNameSuffix+"wselOS",eventWeight);
    }
    if(ttSelection){
      myHistos_->fill1DHistogram("h1DMassTrans"+hNameSuffix+"ttselOS",aPair.getMTMuon(),eventWeight);
      myHistos_->fill1DHistogram("h1DMassTrans"+hNameSuffix+"ttselSS",aPair.getMTMuon(),eventWeight);
      myHistos_->fill1DHistogram("h1DIsoMuon"+hNameSuffix+"ttselOS",aMuon.getProperty(PropertyEnum::combreliso),eventWeight);
    }
    if(mumuSelection){
      myHistos_->fill1DHistogram("h1DPtMET"+hNameSuffix+"mumuselSS",aPair.getMET().Mod(),eventWeight);
      myHistos_->fill1DHistogram("h1DPtMET"+hNameSuffix+"mumuselOS",aPair.getMET().Mod(),eventWeight);
    }
  }
  ///Make QCD shape histograms for specific selection.
  ///Using the same SS/OS scaling factor for now.    
  if(qcdSelectionOS){
    hNameSuffix = sampleName+"qcdselOS";
    if(aPair.getMTMuon()<40) myHistos_->fill1DHistogram("h1DIso"+hNameSuffix,aMuon.getProperty(PropertyEnum::combreliso),eventWeight);
  }

  ///Histograms for the WJet control region. 
  if(wSelection){
    hNameSuffix = sampleName+"wsel";
    if(OS){
      myHistos_->fill1DHistogram("h1DMassTrans"+hNameSuffix+"OS",aPair.getMTMuon(),eventWeight);
      fillDecayPlaneAngle(hNameSuffix+"OS",eventWeight);
    }
    if(SS) myHistos_->fill1DHistogram("h1DMassTrans"+hNameSuffix+"SS",aPair.getMTMuon(),eventWeight);
  }

  ///Histograms for the tt control region
  if(ttSelection){
    hNameSuffix = sampleName+"ttsel";
    if(OS){
      myHistos_->fill1DHistogram("h1DMassTrans"+hNameSuffix+"OS",aPair.getMTMuon(),eventWeight);
      myHistos_->fill1DHistogram("h1DIsoMuon"+hNameSuffix+"OS",aMuon.getProperty(PropertyEnum::combreliso),eventWeight);
    }
    if(SS) myHistos_->fill1DHistogram("h1DMassTrans"+hNameSuffix+"SS",aPair.getMTMuon(),eventWeight);    
  }

  ///Histograms for the DY->mu mu
  if(mumuSelection){
    hNameSuffix = sampleName+"mumusel";
    if(OS) myHistos_->fill1DHistogram("h1DPtMET"+hNameSuffix+"OS",aPair.getMET().Mod(),eventWeight);
    if(SS) myHistos_->fill1DHistogram("h1DPtMET"+hNameSuffix+"SS",aPair.getMET().Mod(),eventWeight);
  }
  
  return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

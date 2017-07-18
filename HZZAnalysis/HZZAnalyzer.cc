#include <sstream>
#include <bitset>

#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooFormulaVar.h"

#include "HZZAnalyzer.h"
#include "HZZHistograms.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
HZZAnalyzer::HZZAnalyzer(const std::string & aName, const std::string & aDecayMode) : Analyzer(aName){

}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
HZZAnalyzer::~HZZAnalyzer(){

        if(myHistos_) delete myHistos_;

}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Analyzer* HZZAnalyzer::clone() const {

        std::string myDecayMode = "4Mu";
        HZZAnalyzer* clone = new HZZAnalyzer(name(),myDecayMode);
        clone->setHistos(myHistos_);
        return clone;

};
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HZZAnalyzer::initialize(TDirectory* aDir,
                             pat::strbitset *aSelections){

        mySelections_ = aSelections;
        myHistos_ = new HZZHistograms(aDir, selectionFlavours_);
        ntupleFile_ = 0;
        hStatsFromFile_ = 0;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HZZAnalyzer::finalize(){

        myHistos_->finalizeHistograms();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HZZAnalyzer::addBranch(TTree *tree){

        std::string leafList = "mass4Mu/f";
        leafList+=":massZ1/f:massZ2/f";
        leafList+=":nMuonsPlus/f:nMuonsMinus/f";
        leafList+=":muon1Pt/f:muon2Pt/f:muon3Pt/f:muon4Pt/f";
        leafList+=":muon1Eta/f:muon2Eta/f:muon3Eta/f:muon4Eta/f";
        leafList+=":muon1Phi/f:muon2Phi/f:muon3Phi/f:muon4Phi/f";
        leafList+=":muon1Dxy/f:muon2Dxy/f:muon3Dxy/f:muon4Dxy/f";
        leafList+=":muon1Dz/f:muon2Dz/f:muon3Dz/f:muon4Dz/f";
        leafList+=":muon1SIP/f:muon2SIP/f:muon3SIP/f:muon4SIP/f";
        leafList+=":muon1Isol/f:muon2Isol/f:muon3Isol/f:muon4Isol/f";
        leafList+=":muon1ID/f:muon2ID/f:muon3ID/f:muon4ID/f";

        tree->Branch("entry4Mu",&aEntry4Mu,leafList.c_str());
      
	leafList = "nMuons/f";
	leafList+=":mass2Mu/f";
	leafList+=":MET/f:mT/f";
	leafList+=":muonPt/f:muonEta/f:muonPhi/f";
	leafList+=":muonSIP/f:muonDxy/f:muonDz/f";
	leafList+=":muonIsol/f:muonID/f";
	leafList+=":nBJets30/f:nLightJets30/f";
	leafList+=":mass2Jets/f:mass3Jets/f";

	tree->Branch("entry1Mu",&aEntry1Mu,leafList.c_str());
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HZZAnalyzer::getPreselectionEff(const EventProxyHTT & myEventProxy){

    if(omp_get_thread_num()!=0) return;

          if(ntupleFile_!=myEventProxy.getTTree()->GetCurrentFile()) {
                ntupleFile_ = myEventProxy.getTTree()->GetCurrentFile();
                if(hStatsFromFile_) delete hStatsFromFile_;
                hStatsFromFile_ = (TH1F*)ntupleFile_->Get("hStats");

                std::string hName = "h1DStats";
                TH1F *hStats = myHistos_->get1DHistogram(hName,true);

                myHistos_->fill1DHistogram("h1DStats",2,std::abs(hStatsFromFile_->GetBinContent(hStatsFromFile_->FindBin(0))));
                myHistos_->fill1DHistogram("h1DStats",3,std::abs(hStatsFromFile_->GetBinContent(hStatsFromFile_->FindBin(2))));
        }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HZZAnalyzer::getSeparatedJets(const EventProxyHTT & myEventProxy, float deltaR){

  mySeparatedJets.clear();

  TLorentzVector muonP4;
  if(myMuons.size()) muonP4 = myMuons[0]->getP4();
  
 for(unsigned int iJet=0; iJet<myEventProxy.jets->size(); ++iJet) {
    const HTTParticle * aJet = &myEventProxy.jets->at(iJet);   
    float dRLeg2 = 999.0;
    float dRLeg1 = 999.0;
    if(muonP4.E()>1) dRLeg1 = muonP4.DeltaR(aJet->getP4());
    bool loosePFJetID = aJet->getProperty(PropertyEnum::PFjetID)>=1;
    if(dRLeg1>deltaR && dRLeg2>deltaR && loosePFJetID) mySeparatedJets.push_back(aJet);
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool HZZAnalyzer::getGoodMuons(const EventProxyHTT & myEventProxy){

  myMuonsPlus.clear();
  myMuonsMinus.clear();
  myMuons.clear();
  bool muonID = false;
  bool singleMuTrigger = false;
  
  for(unsigned int iLepton=0; iLepton<myEventProxy.leptons->size(); ++iLepton) {
    const HTTParticle * aLepton = &myEventProxy.leptons->at(iLepton);    
    int pdgId = aLepton->getProperty(PropertyEnum::PDGId);
    muonID = (int)aLepton->getProperty(PropertyEnum::muonID) & muonIDmask;
    
    singleMuTrigger |= aLepton->hasTriggerMatch(TriggerEnum::HLT_IsoMu22) ||
      aLepton->hasTriggerMatch(TriggerEnum::HLT_IsoTkMu22) ||
      aLepton->hasTriggerMatch(TriggerEnum::HLT_IsoMu22_eta2p1) ||
      aLepton->hasTriggerMatch(TriggerEnum::HLT_IsoTkMu22_eta2p1);
    
    if(aLepton->getP4().Perp()<5 || std::abs(aLepton->getP4().Eta())>2.4) continue;
    if(applyCuts) {
      if(!muonID) continue;
      if(aLepton->getProperty(PropertyEnum::combreliso)>0.35) continue;
      if(std::abs(aLepton->getProperty(PropertyEnum::dxy))>0.5) continue;
      if(std::abs(aLepton->getProperty(PropertyEnum::dz))>1) continue;
      if(std::abs(aLepton->getProperty(PropertyEnum::SIP))>4) continue;
    }
    if(pdgId==-13) myMuonsPlus.push_back(aLepton);
    if(pdgId==13) myMuonsMinus.push_back(aLepton);
    myMuons.push_back(aLepton);
  }
  return singleMuTrigger;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool HZZAnalyzer::fill4MuonTree(){
							      
        float deltaR = 1;
          if(myMuonsPlus.size()>1){
          deltaR = myMuonsPlus[0]->getP4().DeltaR(myMuonsPlus[1]->getP4());
          if(deltaR<0.02) return false;
        }

        if(myMuonsMinus.size()>1){
          deltaR = myMuonsMinus[0]->getP4().DeltaR(myMuonsMinus[1]->getP4());
          if(deltaR<0.02) return false;
        }

        for(auto aMuonPlus: myMuonsPlus) {
                for(auto aMuonMinus: myMuonsMinus) {
                        deltaR = aMuonPlus->getP4().DeltaR(aMuonMinus->getP4());
                        if(deltaR<0.02) return false;
                }
        }

        unsigned int nPt10 = 0, nPt20 = 0;

        if(applyCuts) {
                for(auto it:myMuonsPlus) {
                        if(it->getP4().Perp()>10) ++nPt10;
                        if(it->getP4().Perp()>20) ++nPt20;
                }
                for(auto it:myMuonsMinus) {
                        if(it->getP4().Perp()>10) ++nPt10;
                        if(it->getP4().Perp()>20) ++nPt20;
                }
                if(nPt10<2 || nPt20<1) return false;
        }

        TLorentzVector p4Z1, p4Z2, p4ZZ;
        for(auto it:myMuonsPlus) p4ZZ+=it->getP4();
        for(auto it:myMuonsMinus) p4ZZ+=it->getP4();

        float deltaMass = 9999;
        float deltaTmp = 9999;

        TLorentzVector p4Tmp;
        for(auto aMuonPlus: myMuonsPlus) {
                for(auto aMuonMinus: myMuonsMinus) {
                        p4Tmp = aMuonPlus->getP4() + aMuonMinus->getP4();
                        if(p4Tmp.M()<4) return false;
                        deltaTmp = std::abs(p4Tmp.M() - 91.1896);
                        if(deltaTmp<deltaMass) {
                                deltaMass = deltaTmp;
                                p4Z1 = p4Tmp;
                                p4Z2 = p4ZZ - p4Z1;
                        }
                }
        }

        if(applyCuts) {
                if(p4Z1.M()>120 || p4Z1.M()<12) return false;
                if(p4Z2.M()>120 || p4Z2.M()<12) return false;
                if(p4Z1.M()<40) return false;
        }

        myHistos_->fill1DHistogram("h1DStats",1);

        aEntry4Mu.mass4Mu = p4ZZ.M();
        aEntry4Mu.massZ1 = p4Z1.M();
        aEntry4Mu.massZ2 = p4Z2.M();

        aEntry4Mu.nMuonsPlus = myMuonsPlus.size();
        aEntry4Mu.nMuonsMinus = myMuonsMinus.size();

        if(myMuons.size()>0){
          aEntry4Mu.muon1Pt = myMuons[0]->getP4().Perp();
          aEntry4Mu.muon1Eta = myMuons[0]->getP4().Eta();
          aEntry4Mu.muon1Phi = myMuons[0]->getP4().Phi();
          aEntry4Mu.muon1Isol = myMuons[0]->getProperty(PropertyEnum::combreliso);
          aEntry4Mu.muon1Dxy = myMuons[0]->getProperty(PropertyEnum::dxy);
          aEntry4Mu.muon1Dz = myMuons[0]->getProperty(PropertyEnum::dz);
          aEntry4Mu.muon1SIP = myMuons[0]->getProperty(PropertyEnum::SIP);
          aEntry4Mu.muon1ID = ((int)myMuons[0]->getProperty(PropertyEnum::muonID) & muonIDmask)>0;
        }
        else{
          aEntry4Mu.muon1Pt = -99;
          aEntry4Mu.muon1Eta = -99;
          aEntry4Mu.muon1Phi = -99;
          aEntry4Mu.muon1Isol = -99;
          aEntry4Mu.muon1Dxy = -99;
          aEntry4Mu.muon1Dz = -99;
          aEntry4Mu.muon1SIP = -99;
          aEntry4Mu.muon1ID = -99;
        }
        if(myMuons.size()>1){
          aEntry4Mu.muon2Pt = myMuons[1]->getP4().Perp();
          aEntry4Mu.muon2Eta = myMuons[1]->getP4().Eta();
          aEntry4Mu.muon2Phi = myMuons[1]->getP4().Phi();
          aEntry4Mu.muon2Isol = myMuons[1]->getProperty(PropertyEnum::combreliso);
          aEntry4Mu.muon2Dxy = myMuons[1]->getProperty(PropertyEnum::dxy);
          aEntry4Mu.muon2Dz = myMuons[1]->getProperty(PropertyEnum::dz);
          aEntry4Mu.muon2SIP = myMuons[1]->getProperty(PropertyEnum::SIP);
          aEntry4Mu.muon2ID = ((int)myMuons[1]->getProperty(PropertyEnum::muonID) & muonIDmask)>0;
        }
        else{
          aEntry4Mu.muon2Pt = -99;
          aEntry4Mu.muon2Eta = -99;
          aEntry4Mu.muon2Phi = -99;
          aEntry4Mu.muon2Isol = -99;
          aEntry4Mu.muon2Dxy = -99;
          aEntry4Mu.muon2Dz = -99;
          aEntry4Mu.muon2SIP = -99;
          aEntry4Mu.muon2ID = -99;
        }
        if(myMuons.size()>2){
          aEntry4Mu.muon3Pt = myMuons[2]->getP4().Perp();
          aEntry4Mu.muon3Eta = myMuons[2]->getP4().Eta();
          aEntry4Mu.muon3Phi = myMuons[2]->getP4().Phi();
          aEntry4Mu.muon3Isol = myMuons[2]->getProperty(PropertyEnum::combreliso);
          aEntry4Mu.muon3Dxy = myMuons[2]->getProperty(PropertyEnum::dxy);
          aEntry4Mu.muon3Dz = myMuons[2]->getProperty(PropertyEnum::dz);
          aEntry4Mu.muon3SIP = myMuons[2]->getProperty(PropertyEnum::SIP);
          aEntry4Mu.muon3ID = ((int)myMuons[2]->getProperty(PropertyEnum::muonID) & muonIDmask)>0;
        }
        else{
          aEntry4Mu.muon3Pt = -99;
          aEntry4Mu.muon3Eta = -99;
          aEntry4Mu.muon3Phi = -99;
          aEntry4Mu.muon3Isol = -99;
          aEntry4Mu.muon3Dxy = -99;
          aEntry4Mu.muon3Dz = -99;
          aEntry4Mu.muon3SIP = -99;
          aEntry4Mu.muon3ID = -99;
        }
        if(myMuons.size()>3){
          aEntry4Mu.muon4Pt = myMuons[3]->getP4().Perp();
          aEntry4Mu.muon4Eta = myMuons[3]->getP4().Eta();
          aEntry4Mu.muon4Phi = myMuons[3]->getP4().Phi();
          aEntry4Mu.muon4Isol = myMuons[3]->getProperty(PropertyEnum::combreliso);
          aEntry4Mu.muon4Dxy = myMuons[3]->getProperty(PropertyEnum::dxy);
          aEntry4Mu.muon4Dz = myMuons[3]->getProperty(PropertyEnum::dz);
          aEntry4Mu.muon4SIP = myMuons[3]->getProperty(PropertyEnum::SIP);
          aEntry4Mu.muon4ID = ((int)myMuons[3]->getProperty(PropertyEnum::muonID) & muonIDmask)>0;
        }
        else{
          aEntry4Mu.muon4Pt = -99;
          aEntry4Mu.muon4Eta = -99;
          aEntry4Mu.muon4Phi = -99;
          aEntry4Mu.muon4Isol = -99;
          aEntry4Mu.muon4Dxy = -99;
          aEntry4Mu.muon4Dz = -99;
          aEntry4Mu.muon4SIP = -99;
          aEntry4Mu.muon4ID = -99;
        }

        myHistos_->fill1DHistogram("h1DMassZ1",p4Z1.M());
        myHistos_->fill1DHistogram("h1DMassZ2",p4Z2.M());
        myHistos_->fill1DHistogram("h1DMass4Mu",p4ZZ.M());

	return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool HZZAnalyzer::fill1MuonTree(){

        int nBJets = 0;
        TLorentzVector bJet1, bJet2;
        for(auto itJet: mySeparatedJets) {
          if(std::abs(itJet->getP4().Eta())<2.4 &&
             itJet->getP4().Pt()>30 && 
             itJet->getProperty(PropertyEnum::bCSVscore)>0.8484){
            ++nBJets;
            if(bJet1.E()<1) bJet1 = itJet->getP4();
            else if(bJet2.E()<1) bJet2 = itJet->getP4();
          }
        }
        int nLightJets = 0;
        TLorentzVector lightJet1, lightJet2;
        for(auto itJet: mySeparatedJets) {
          if(itJet->getP4().Pt()>30 && 
             itJet->getProperty(PropertyEnum::bCSVscore)<0.8484 &&
             (bJet1.E()<1 || itJet->getP4().DeltaR(bJet1)>0.5) && 
             (bJet2.E()<1 || itJet->getP4().DeltaR(bJet2)>0.5)){
            ++nLightJets;
            if(lightJet1.E()<1) lightJet1 = itJet->getP4();
            else if(lightJet2.E()<1) lightJet2 = itJet->getP4();
          }
        }
        float mass2Jets = (lightJet1 + lightJet2).M();
        float mass3Jets = (bJet1 + lightJet1 + lightJet2).M();

	float mT2 = -99;
	aEntry1Mu.nMuons = myMuons.size();       
	if(myMuons.size()){
	  const HTTParticle *aMuon = myMuons[0];

	  mT2 = pow(aMuon->getP4().Et() + myMET.Et(),2) - 
	        pow(aMuon->getP4().X() + myMET.X(),2) - 
	        pow(aMuon->getP4().Y() + myMET.Y(),2);
	  aEntry1Mu.muonPt = aMuon->getP4().Pt();
	  aEntry1Mu.muonEta = aMuon->getP4().Eta();
	  aEntry1Mu.muonPhi = aMuon->getP4().Phi();
	  aEntry1Mu.muonIsol = aMuon->getProperty(PropertyEnum::combreliso);
	  aEntry1Mu.muonID = ((int)aMuon->getProperty(PropertyEnum::muonID) & muonIDmask)>0;
	  aEntry1Mu.muonSIP = aMuon->getProperty(PropertyEnum::SIP);
	  aEntry1Mu.muonDxy = aMuon->getProperty(PropertyEnum::dxy);
	  aEntry1Mu.muonDz = aMuon->getProperty(PropertyEnum::dz);
	}
	else{
	  aEntry1Mu.muonPt = -99;
	  aEntry1Mu.muonEta = -99;
	  aEntry1Mu.muonPhi = -99;
	  aEntry1Mu.muonIsol = -99;
	  aEntry1Mu.muonID = -99;
	  aEntry1Mu.muonSIP = -99;
	  aEntry1Mu.muonDxy = -99;
	  aEntry1Mu.muonDz = -99;

	}
	if(myMuonsPlus.size() && myMuonsMinus.size()){
	  aEntry1Mu.mass2Mu = (myMuonsPlus[0]->getP4() + myMuonsMinus[0]->getP4()).M();
	}
	else aEntry1Mu.mass2Mu = 0;

        aEntry1Mu.MET = myMET.Pt();
        aEntry1Mu.mT = sqrt(mT2);
        aEntry1Mu.nBJets30 = nBJets;
        aEntry1Mu.nLightJets30 = nLightJets;
        aEntry1Mu.mass2Jets = mass2Jets;
        aEntry1Mu.mass3Jets = mass3Jets;

        return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool HZZAnalyzer::analyze(const EventProxyBase& iEvent){

        const EventProxyHTT & myEventProxy = static_cast<const EventProxyHTT&>(iEvent);
        std::string fileName = myEventProxy.getTTree()->GetCurrentFile()->GetName();
        getPreselectionEff(myEventProxy);
        myHistos_->fill1DHistogram("h1DStats",0);

        bool singleMuTrigger = getGoodMuons(myEventProxy);      
        if(!singleMuTrigger) return false;
	if(myMuons.size()<1) return false;

	getSeparatedJets(myEventProxy, 0.4);
	myMET = TLorentzVector(myEventProxy.event->getMET().X(),
			       myEventProxy.event->getMET().Y(),
			       0,
			       myEventProxy.event->getMET().Mod());

	//if(myMuonsPlus.size()!=2 || myMuonsMinus.size()!=2) return false;
	//fill4MuonTree();

        //if(mySeparatedJets.size()<2) return false;
	fill1MuonTree();

        return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

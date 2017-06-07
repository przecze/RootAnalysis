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
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HZZAnalyzer::finalize(){

        myHistos_->finalizeHistograms();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void HZZAnalyzer::addBranch(TTree *tree){

        std::string leafList = "mass4Mu/f:massZ1/f:massZ2/f";
        leafList+=":muon1ID/f:muon2ID/f:muon3ID/f:muon4ID/f";
        leafList+=":muon1Isolation/f:muon2Isolation/f:muon3Isolation/f:muon4Isolation/f";
        leafList+=":muon1Pt/f:muon2Pt/f:muon3Pt/f:muon4Pt/f";
        leafList+=":muon1SIP/f:muon2SIP/f:muon3SIP/f:muon4SIP/f";

        tree->Branch("entry",&aEntry,leafList.c_str());

}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool HZZAnalyzer::analyze(const EventProxyBase& iEvent){

        const EventProxyHTT & myEventProxy = static_cast<const EventProxyHTT&>(iEvent);
        std::string fileName = myEventProxy.getTTree()->GetCurrentFile()->GetName();

        bool applyCuts = true;

        bool singleMuTrigger = false;
        bool muonID = true;
        unsigned int muonIDmask = (1<<6);
        std::vector<const HTTParticle *> myMuonsPlus, myMuonsMinus, myMuons;
        for(unsigned int iLepton=0; iLepton<myEventProxy.leptons->size(); ++iLepton) {
                const HTTParticle * aLepton = &myEventProxy.leptons->at(iLepton);

		myHistos_->fill1DHistogram("h1DPtMu1",aLepton->getP4().Perp());
	}

        return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "TestAnalyzer.h"
#include "TestHistograms.h"
#include "EventProxyTest.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
TestAnalyzer::TestAnalyzer(const std::string & aName):Analyzer(aName){ tmpName = "h1DXSignal";}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
TestAnalyzer::~TestAnalyzer(){ if(myHistos_) delete myHistos_; }
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Analyzer* TestAnalyzer::clone() const{

  TestAnalyzer* clone = new TestAnalyzer(name());
  clone->setHistos(myHistos_);
  return clone;

};
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void TestAnalyzer::initialize(TDirectory* aDir,
			      pat::strbitset *aSelections){

  mySelections_ = aSelections;

  ///The histograms for this analyzer will be saved into "TestAnalyzer"
  ///directory of the ROOT file
  ///NOTE: due to a bug hists land in the Summary directory
  myHistos_ = new TestHistograms(aDir, selectionFlavours_);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void TestAnalyzer::finalize(){

  myHistos_->finalizeHistograms(0,1.0);

}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool TestAnalyzer::analyze(const EventProxyBase& iEvent){

  const EventProxyTest & myEventProxy = static_cast<const EventProxyTest&>(iEvent);

  unsigned int nJets = myEventProxy.getNumberOfJets();
  std::cout<<"Number of jets: "<<nJets<<std::endl;

  std::cout<<"patTriggerObjectStandAlones_patTrigger__PAT_present: "
  <<myEventProxy.patTriggerObjectStandAlones_patTrigger__PAT_present
  <<" "<<myEventProxy.patTriggerObjectStandAlones_patTrigger__PAT_obj_
  <<std::endl;

  for(auto vTypes: myEventProxy.patTriggerObjectStandAlones_patTrigger__PAT_obj_triggerObjectTypes_){
    if(vTypes.size()) std::cout<<vTypes.size()<<std::endl;
  }

    for(unsigned int iIndex=0;iIndex<myEventProxy.kMaxpatTriggerObjectStandAlones_patTrigger__PAT_obj;++iIndex){
      int aSize = myEventProxy.patTriggerObjectStandAlones_patTrigger__PAT_obj_triggerObjectTypes_[iIndex].size();
      aSize = myEventProxy.patTriggerObjectStandAlones_patTrigger__PAT_obj_filterLabels_[iIndex].size();
      if(aSize) std::cout<<"aSize: "<<aSize<<std::endl;
      for(unsigned int iIndex1=0;iIndex1<aSize;++iIndex1){
        int type = myEventProxy.patTriggerObjectStandAlones_patTrigger__PAT_obj_triggerObjectTypes_[iIndex][iIndex1];
        std::cout<<"trigger type:"<<type<<std::endl;
        if(type==90){
          std::cout<<"filter: "<<myEventProxy.patTriggerObjectStandAlones_patTrigger__PAT_obj_filterLabels_[iIndex][iIndex1];
        }
      }
    }

/*
  for(unsigned int iJet=0;iJet<nJets;++iJet){
    std::cout<<iJet<<" jet pt: "<<myEventProxy.getJetP4(iJet).Perp()<<std::endl;
  }
*/
  return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

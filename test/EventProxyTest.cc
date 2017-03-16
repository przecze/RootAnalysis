#include "EventProxyTest.h"

#include "TLeaf.h"

#include <iostream>

EventProxyTest::EventProxyTest(){}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
EventProxyTest::~EventProxyTest(){}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
EventProxyBase* EventProxyTest::clone() const{

  return new EventProxyTest();
  
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void EventProxyTest::init(std::vector<std::string> const& iFileNames){

	treeName_ = "Events";

	EventProxyBase::init(iFileNames);

	fChain->SetMakeClass(1);
	fChain->SetBranchStatus("*",1);

	fChain->SetBranchAddress("floats_jetNtupleMaker_jetEta_jetAnalysis.obj", &floats_jetNtupleMaker_jetEta_jetAnalysis_obj);	
	fChain->SetBranchAddress("floats_jetNtupleMaker_jetMass_jetAnalysis.obj", &floats_jetNtupleMaker_jetMass_jetAnalysis_obj);
	fChain->SetBranchAddress("floats_jetNtupleMaker_jetPhi_jetAnalysis.obj", &floats_jetNtupleMaker_jetPhi_jetAnalysis_obj);
	fChain->SetBranchAddress("floats_jetNtupleMaker_jetPt_jetAnalysis.obj", &floats_jetNtupleMaker_jetPt_jetAnalysis_obj);

}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
TLorentzVector EventProxyTest::getJetP4(unsigned int iJet) const {

  TLorentzVector aP4;
  
  if(iJet>=floats_jetNtupleMaker_jetPt_jetAnalysis_obj.size()) return aP4;
  
  aP4.SetPtEtaPhiM(floats_jetNtupleMaker_jetPt_jetAnalysis_obj[iJet],
		   floats_jetNtupleMaker_jetEta_jetAnalysis_obj[iJet],
		   floats_jetNtupleMaker_jetPhi_jetAnalysis_obj[iJet],
		   floats_jetNtupleMaker_jetMass_jetAnalysis_obj[iJet]);
  
    return aP4;
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

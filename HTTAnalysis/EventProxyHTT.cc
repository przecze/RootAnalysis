#include "EventProxyHTT.h"

#include "TSystem.h"

#include <iostream>

EventProxyHTT::EventProxyHTT(){}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
EventProxyHTT::~EventProxyHTT(){}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
EventProxyBase* EventProxyHTT::clone() const{

  return new EventProxyHTT();
  
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void EventProxyHTT::init(std::vector<std::string> const& iFileNames){

  treeName_ = "HTauTauTree";
  
  EventProxyBase::init(iFileNames);
  fChain->SetMakeClass(0);
  
  event = 0;
  pairs = 0;
  jets = 0;
  genLeptons = 0;
  
  fChain->SetBranchAddress("HTTEvent.",&event);
  fChain->SetBranchAddress("HTTPairCollection",&pairs);
  fChain->SetBranchAddress("HTTJetCollection",&jets);
  fChain->SetBranchAddress("HTTGenLeptonCollection",&genLeptons);
  
  fChain->SetBranchStatus("*",1);
  fChain->SetBranchStatus("HTTEvent.*",1);
  fChain->SetBranchStatus("HTTPairCollection.*",1);
  fChain->SetBranchStatus("HTTPairCollection.p4",1);
  fChain->SetBranchStatus("HTTPairCollection.leg1.p4",1);
  fChain->SetBranchStatus("HTTPairCollection.leg1.properties",1);
  fChain->SetBranchStatus("HTTPairCollection.leg2.p4",1);
  fChain->SetBranchStatus("HTTPairCollection.leg2.properties",1);
  fChain->SetBranchStatus("HTTPairCollection.mtLeg1",1);
  fChain->SetBranchStatus("HTTPairCollection.mtLeg2",1);  
  fChain->SetBranchStatus("HTTJetCollection*",1);  
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void  EventProxyHTT::enableBranches(){
  
  fChain->SetBranchStatus("*",1);
  
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void  EventProxyHTT::disableBranches(){
  
  fChain->SetBranchStatus("*",0);
  
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void  EventProxyHTT::clear(){
  
  event->clear();
  pairs->clear();
  
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

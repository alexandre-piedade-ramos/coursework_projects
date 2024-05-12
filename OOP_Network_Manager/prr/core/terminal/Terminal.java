package prr.core.terminal;

import prr.core.client.*;
import prr.core.communications.*;
import prr.core.exception.SameStateException;
import prr.core.exception. NoOngoingCommunicationException;
import prr.core.terminal.terminalstate.*;
import prr.core.notifications.*;
import java.io.Serializable;


import java.util.Collection;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;
import java.util.Iterator;

abstract public class Terminal implements Serializable,StateObserverSubject /* FIXME maybe addd more interfaces */{

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202208091753L;
  

  private Client _owner;
  private String _id;

  private TerminalState _state;
  private TerminalState _previousState;
  private double _debts;
  private double _payments;
  private Map<String,Terminal> _friends = 
                                new TreeMap<>( String.CASE_INSENSITIVE_ORDER);
                                //comparador de strings
  private Client[] _toNotify;
  private List<Comm> _receivedComms;
  private List<Comm> _sentComms;
  private InteractiveComm _ongoingComm;
  private List<TerminalStateObserver> _observers;

  
  
  public Terminal(String id, Client owner){
    _owner=owner;
    _id=id;
  
    _debts=0;
    _payments=0;
    _state= new IdleState(this);
    _receivedComms= new ArrayList<>();
    _sentComms= new ArrayList<>();
    _ongoingComm=null;
    _observers=new ArrayList<>();
  }

  public String getId(){
    return _id;
  }
  /**
   * Checks if this terminal can end the current interactive communication.
   *
   * @return true if this terminal is busy (i.e., it has an active interactive communication) and
   *          it was the originator of this communication.
   **/
  public boolean canEndCurrentCommunication() {// FIXME add implementation code
    return _ongoingComm==null?false:_ongoingComm.getFrom().getId().equals(_id); 
  }
  
  /**
   * Checks if this terminal can start a new communication.
   *
   * @return true if this terminal is neither off neither busy, false otherwise.
   **/
  public boolean canStartCommunication() { // FIXME add implementation code
    return _state.canStartCommunication();
  }

  public double paidBalance(){ 
    return 0;
  }

  public boolean isFriend(Terminal t){
    return _friends.containsKey(t.getId());
  }
  public void addFriend(Terminal friend){
    if(!(friend.getId().equals(_id))){
      if(!isFriend(friend)){
        _friends.put(friend.getId(),friend);
      }
    }
  }

  public void rmvFriend(Terminal friend){
    _friends.remove(friend.getId());
  }

  public String friendsToString(){
    
    String toReturn="";

    if(_friends.size()>0) {toReturn+="|";}
    else{return toReturn;}

    for(Terminal t : _friends.values()){
      if(toReturn.equals("|")){
        toReturn+=t.getId();
      }
      else{
        toReturn+="," + t.getId();
      }
      
    }

    return toReturn;
  }
  public abstract String getTypeName();
  @Override
  public String toString(){
    String type = this.getTypeName();
    return type + "|" + _id + "|"+ _owner.getKey() +"|" + _state.toString() +"|" +
      Math.round(_payments) +"|" + Math.round(_debts) + friendsToString();
  }

  @Override
  public boolean equals(Object other){
    if(!(other instanceof Terminal)) return false;

    Terminal otherTerminal = (Terminal) other;

    return _id==otherTerminal.getId();
  }

  public void setState(TerminalState state){
    _state=state;

  }

  public TerminalState getState(){
    return _state;

  }

  public TerminalState getPreviousState(){
    return _previousState;

  }

  public boolean inactive(){ 
    return _sentComms.size()==0 && _receivedComms.size()==0 ;
  }

  public boolean toSilent() throws SameStateException{

    return _state.StateToSilent();
  }

  public boolean toOff()throws SameStateException{
    return _state.StateToOff();
  }

  public boolean toBusy()throws SameStateException{

    return _state.StateToBusy();
  }

  public boolean toIdle()throws SameStateException{

    return _state.StateToIdle();
  }

  public double getDebts(){
    return _debts;
  }

  public double getPayments(){
    return _payments;
  }

  public Client getClient(){
    return _owner;
  }


  public TextComm sendTextComm(int id,Terminal to, String text){
    TextComm newComm =new TextComm(id, this, to, text);
    _sentComms.add(newComm);
    double cost=newComm.getCost();
    _debts+=cost;
    _owner.addDebt(cost);
    return newComm;
  }

  public void acceptTextComm(Comm toAccept){
    _receivedComms.add(toAccept);
  }

  public VoiceComm sendVoiceComm(int id,Terminal to)throws SameStateException{
    this.toBusy();
    VoiceComm newComm =new VoiceComm(id, this, to);
    _sentComms.add(newComm);
    _ongoingComm=newComm;
    return newComm;
  }

  public void acceptVoiceComm(VoiceComm toAccept)throws SameStateException{
    this.toBusy();
    _receivedComms.add(toAccept);
    _ongoingComm=toAccept;
  }

  public VideoComm sendVideoComm(int id,Terminal to)throws SameStateException{
    this.toBusy();
    VideoComm newComm =new VideoComm(id, this, to);
    _sentComms.add(newComm);
    _ongoingComm=newComm;
    return newComm;
  }

  public void acceptVideoComm(VideoComm toAccept)throws SameStateException{
    this.toBusy();
    _receivedComms.add(toAccept);
    _ongoingComm=toAccept;
  }
  public abstract boolean VideoCallCompatible();
  
  public void callFinished()throws SameStateException{
    if(_previousState instanceof SilentState) this.toSilent();
    if(_previousState instanceof IdleState) this.toIdle();
    _ongoingComm=null;
  }
  public double endOngoingCall(int durationMin){
    double cost=-1;
    try{
    _ongoingComm.endInteractiveComm(durationMin);
    cost = _ongoingComm.computeCost();
    _ongoingComm.getTo().callFinished();
    this.callFinished();
    _debts+=cost;
    _owner.addDebt(cost);
    
    }catch(SameStateException exc){
      
      exc.printStackTrace();
  
    }
    finally{return cost;}
  }

  public void setPreviousState(TerminalState state){
    _previousState=state;
  }

  public String showOngoingComm() throws NoOngoingCommunicationException{
    if(_ongoingComm==null) throw new NoOngoingCommunicationException();
    
    return _ongoingComm.toString();
  }

  public void addStateObserver(TerminalStateObserver obs){
    _observers.add(obs);
  }

  public void rmvStateObserver(TerminalStateObserver obs){
    _observers.remove(obs);
  }

  public void updateStateObservers(){
    Iterator<TerminalStateObserver> itr = _observers.iterator();

    while(itr.hasNext()){
      TerminalStateObserver obs = itr.next();
      obs.update();
      if(!obs.stillObserving()) itr.remove();
    }
  }

  public boolean sentComm(Comm c){
    return _sentComms.contains(c);
  }

  public void addDebt(double val){
    _debts+=val;

  }

  public void addPayment(double val){
    _debts-=val;
    _payments+=val;
  }
}

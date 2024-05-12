package prr.core;

import prr.core.client.*;
import prr.core.terminal.*;

import java.io.Serializable;
import java.io.IOException;
import prr.core.exception.*;
import prr.core.comparator.*;
import prr.core.communications.*;
import prr.core.notifications.*;

import java.util.Collection;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.LinkedList;


// FIXME add more import if needed (cannot import from pt.tecnico or prr.app)

/**
 * Class Store implements a store.
 */
public class Network implements Serializable {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202208091753L;
  
  // FIXME define attributes
  // FIXME define contructor(s)
  // FIXME define methods
  
  /**
   * Read text input file and create corresponding domain entities.
   * 
   * @param filename name of the text input file
   * @throws UnrecognizedEntryException if some entry is not correct
   * @throws IOException if there is an IO erro while processing the text file
   */
  void importFile(String filename) throws UnrecognizedEntryException, IOException /* FIXME maybe other exceptions */  {
    Parser parser = new Parser(this);
    parser.parseFile(filename);
  }
  //While copy operations are much more expensive, find requests are much more
  //common. HashMaps are used here for this reason, but I'm not sure that TreeMaps wouldnt be a better choice.
  private Map<String,Client> _clients = new HashMap<>();
  private Map<String,Terminal> _terminals = new HashMap<>();
  private List<Comm> _comms= new ArrayList<>();
  private double _debts;
  private double _payments;

  public void registerClient(String key, String name, Integer taxNumber) 
        throws DuplicateClientKeyExceptionCore{

          if(containsClient(key)){
            throw new DuplicateClientKeyExceptionCore(key);
          }

          _clients.put(key.toLowerCase(), new Client(key, name, taxNumber));  
  }

  public boolean containsClient(String key){
    return _clients.containsKey(key.toLowerCase());
  }

  public Client fetchClient(String key) throws UnknownClientKeyExceptionCore{
    Client c= _clients.get(key);
    if(c==null) throw new UnknownClientKeyExceptionCore(key);

    return c;
  }

  public List<Client> sortedClientsbyKey(){

    ArrayList<Client> clients = new ArrayList<>(_clients.values());
    clients.sort(new ClientKeyComparator());

    return clients;
  }
  
  public String clientToString(String key) 
        throws UnknownClientKeyExceptionCore{

    Client c = this.fetchClient(key);

    return c.toString();
  }

  public List<String> clientNotifsAsStrings(String key)
      throws UnknownClientKeyExceptionCore{
    
    Client c = this.fetchClient(key);    

    return c.notifsToStrings();
  }

  public boolean validTerminalId(String id){
    //return id.matches(/[0-9]{6}/); 
    //return id.matches(/\d{6}/); 

    if(id.length()!=6) return false;
    for(int i=0; i<6; i++){
      if(!Character.isDigit(id.charAt(i))){
        return false;
      }
    }

    return true;
  }

  public void registerTerminal(String id, String type, String ownerKey) throws 
      DuplicateTerminalKeyExceptionCore, InvalidTerminalKeyExceptionCore,
        UnknownClientKeyExceptionCore{

    if(!validTerminalId(id)) throw new InvalidTerminalKeyExceptionCore(id);
    if(containsTerminal(id)) throw new DuplicateTerminalKeyExceptionCore(id);
    if(!containsClient(ownerKey)) throw new UnknownClientKeyExceptionCore(ownerKey);

    Client owner =fetchClient(ownerKey);
    Terminal terminal;
    if(type.equals(TerminalType.BASIC.toString())){
      terminal= new BasicTerminal(id, owner );

    }
    else if(type.equals(TerminalType.FANCY.toString())){
      terminal= new FancyTerminal(id, owner);
    }
    else{ return;} //aint happening

    _terminals.put(id , terminal);
    owner.addTerminal(terminal);
  }

  public boolean containsTerminal(String id){
    return _terminals.containsKey(id);
  }
  
  public List<Terminal> sortedTerminalsbyId(){
    ArrayList<Terminal> terminals = new ArrayList<>(_terminals.values());
    terminals.sort(new TerminalIdComparator());

    return terminals;
  }

  public Terminal fetchTerminal(String key) throws 
                                      UnknownTerminalKeyExceptionCore{
    Terminal t= _terminals.get(key);
    if(t==null) throw new UnknownTerminalKeyExceptionCore(key);

    return t;
  }

  public List<Terminal> inactiveTerminals(){
    ArrayList<Terminal> inactiveTerminals = new ArrayList<>();


    _terminals.forEach((id, terminal) -> {

      if(terminal.inactive()){
        inactiveTerminals.add(terminal);
      }
    });

    inactiveTerminals.sort(new TerminalIdComparator());

    return inactiveTerminals;

  }

  public void addFriend(String terminal, String friend) 
                        throws UnknownTerminalKeyExceptionCore{

    Terminal t = fetchTerminal(terminal);
    Terminal friendT= fetchTerminal(friend);

    t.addFriend(friendT);


  }

  public void rmvFriend(String terminal, String friend) 
                        throws UnknownTerminalKeyExceptionCore{

    Terminal t = fetchTerminal(terminal);
    Terminal friendT= fetchTerminal(friend);

    t.rmvFriend(friendT);
  }



  public double newTextComm(Terminal from, String toId, String text)
                        throws DestinationIsOffException,UnknownTerminalKeyExceptionCore{
    
    Terminal to = this.fetchTerminal(toId);
    
      try{
        if(!to.getState().canAcceptTextCommunication()){
          return -1; //Should't run
        }
      }catch(Exception exc){
          if(from.getClient().notifsOn()){
            to.addStateObserver(new TextCommNotification(to,from));
          }
          throw exc;
      }
      TextComm newComm=from.sendTextComm(_comms.size()+1,to, text);
      _comms.add(newComm);
      to.acceptTextComm(newComm);
      double cost=newComm.getCost();
      _debts+=cost;
      return cost;
  }

  public void newInteractiveComm(Terminal from, String toId, String type)
        throws DestinationIsSilentException, DestinationIsOffException,
          DestinationIsBusyException, UnsupportedAtDestinationException, 
            UnsupportedAtOriginException,UnknownTerminalKeyExceptionCore{

    try{
    Terminal to = fetchTerminal(toId);
    if(to==from) return;
      try{
        if(!to.getState().canAcceptInteractiveCommunication()){
          return;
        }
      }catch(Exception exc){
        if(from.getClient().notifsOn()){
          to.addStateObserver(new InteractiveCommNotification(to,from));
        }
        throw exc;
      }
    if(type.equals(CommNames.VOICE.name())){
      VoiceComm newComm=from.sendVoiceComm(_comms.size()+1,to);
      _comms.add(newComm);
      to.acceptVoiceComm(newComm);
      
    }
    else if(type.equals(CommNames.VIDEO.name())){
      if(!from.VideoCallCompatible()){
        throw new UnsupportedAtOriginException(from.getId(), CommNames.VIDEO.name());
      }
      if(!to.VideoCallCompatible()){
        throw new UnsupportedAtDestinationException(to.getId(), CommNames.VIDEO.name());
      }

      VideoComm newComm=from.sendVideoComm(_comms.size()+1,to);
      _comms.add(newComm);
      to.acceptVideoComm(newComm);

      return;
    }
    else{return;}
    }
    catch(SameStateException exc){
      exc.printStackTrace();

    }
  }

  public boolean enableClientNotifs(String key) throws UnknownClientKeyExceptionCore{
    return this.fetchClient(key).enableNotifs();
  }
  public boolean disableClientNotifs(String key) throws UnknownClientKeyExceptionCore{
    return this.fetchClient(key).disableNotifs();
  }
  
  public int getGlobalDebts(){
    return (int) Math.round(_debts);
  }

  public int getGlobalPayments(){
    return (int) Math.round(_payments);
  }

  public int getClientDebts(String key) throws UnknownClientKeyExceptionCore{
    return (int) Math.round(this.fetchClient(key).getDebts());
  }

  public int getClientPayments(String key) throws UnknownClientKeyExceptionCore{
    return (int) Math.round(this.fetchClient(key).getPayments());
  }

  public List<String> getAllCommsAsStrings(){
    LinkedList<String> toReturn = new LinkedList<>();
    for(Comm c : _comms){
      toReturn.add(c.toString());
    }
    return toReturn;
  }

  public List<String> getClientReceivedCommsAsStrings(String key)
                          throws UnknownClientKeyExceptionCore{
    Client client=this.fetchClient(key);
    List<Comm> clientComms= client.getComms();
    LinkedList<String> toReturn= new LinkedList<>();
    for(Comm c : clientComms){
      if(c.getTo().getClient().getKey().equals(key)){
        toReturn.add(c.toString());
      }
    }
    return toReturn;
  }

  public List<String> getClientSentCommsAsStrings(String key)
                              throws UnknownClientKeyExceptionCore{
    Client client=this.fetchClient(key);
    List<Comm> clientComms= client.getComms();
    LinkedList<String> toReturn=new LinkedList<>();
    for(Comm c : clientComms){
      if(c.getFrom().getClient().getKey().equals(key)){
        toReturn.add(c.toString());
      }
    }
    return toReturn;
  }

  public List<String> getClientsWithoutDebt(){

    LinkedList<String> toReturn=new LinkedList<>();
    for(Client c : _clients.values()){
      if(c.getDebts()==0){toReturn.add(c.toString());}
    }
    return toReturn;
  }

  public List<Client> getClientsWithDebt(){
    ArrayList<Client> toReturn=new ArrayList<>();
    for(Client c : _clients.values()){
      if(c.getDebts()>0){toReturn.add(c);}
    }
    toReturn.sort((c1,c2) ->{
        if(c1.getDebts()==c2.getDebts()){
          return c1.getKey().compareTo(c2.getKey());
        }else{
          return (int) ((c1.getDebts()-c2.getDebts())*-1);//for descending order
        }
      }
    );

    return toReturn;
  }


  public List<Terminal> getTerminalsWithNegativeBalance(){

      ArrayList<Terminal> toReturn=new ArrayList<>();
      for(Terminal t : _terminals.values()){
        if(t.getDebts()<t.getPayments()){toReturn.add(t);}
      }

      toReturn.sort(new TerminalIdComparator());
      return toReturn;
    }
  
  public Comm fetchComm(int id) throws InvalidCommunicationException{
    if(id>_comms.size()) throw new InvalidCommunicationException(id);

    return _comms.get(id-1);
  }
  public void payComm(int id, Terminal t)throws InvalidCommunicationException{
    Comm comm = this.fetchComm(id);
    if(!t.sentComm(comm)) throw new InvalidCommunicationException(id);
    if(!comm.isFinished()) return;
    if(!comm.isPaid()) return;

    double val = comm.payComm();
    _debts-=val;
    _payments+=val;

  }
  
}





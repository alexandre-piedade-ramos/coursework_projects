package prr.core.terminal.terminalstate;
import prr.core.terminal.Terminal;

import java.io.Serializable;

import prr.core.exception.*;

public class IdleState extends TerminalState implements Serializable{

    private static final long serialVersionUID = 202208091753L;

    public IdleState(Terminal terminal){
        super(terminal);
    }

    public boolean StateToIdle() throws SameStateException{
        throw new SameStateException();
    }
    public boolean StateToOff(){
        Terminal terminal= this.getTerminal();
        terminal.setState(new OffState(terminal)) ;
        terminal.setPreviousState(this);
        terminal.updateStateObservers();
        return true;
    }
    public boolean StateToSilent(){
        Terminal terminal= this.getTerminal();
        terminal.setState(new SilentState(terminal)) ;
        terminal.setPreviousState(this);
        terminal.updateStateObservers();
        return true;
    }
    public boolean StateToBusy(){
        Terminal terminal= this.getTerminal();
        terminal.setState(new BusyState(terminal)) ;
        terminal.setPreviousState(this);
        terminal.updateStateObservers();
        return true;
    }
    public String toString(){
        return TerminalMode.IDLE.name();
    }
    public boolean canStartCommunication(){
        return true;
    }

    public boolean canAcceptTextCommunication() throws  DestinationIsOffException{
        return true;
    }
    public boolean canAcceptInteractiveCommunication()throws DestinationIsSilentException, DestinationIsOffException, DestinationIsBusyException{
        return true;
    }
}
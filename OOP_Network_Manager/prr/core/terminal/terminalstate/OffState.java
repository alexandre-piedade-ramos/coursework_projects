package prr.core.terminal.terminalstate;
import prr.core.terminal.Terminal;

import java.io.Serializable;

import prr.core.exception.*;

public class OffState extends TerminalState implements Serializable{

    private static final long serialVersionUID = 202208091753L;

    public OffState(Terminal terminal){
        super(terminal);
    }

    public boolean StateToOff() throws SameStateException{
        throw new SameStateException();
    }
    public boolean StateToIdle(){
        Terminal terminal= this.getTerminal();
        terminal.setState(new IdleState(terminal)) ;
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
        return false;
    }
    public String toString(){
        return TerminalMode.OFF.name();
    }
    public boolean canStartCommunication(){
        return false;
    }

    public boolean canAcceptTextCommunication() throws  DestinationIsOffException{
        throw new DestinationIsOffException(this.getTerminal().getId());
    }
    public boolean canAcceptInteractiveCommunication()throws DestinationIsSilentException, DestinationIsOffException, DestinationIsBusyException{
        throw new DestinationIsOffException(this.getTerminal().getId());
    }
}
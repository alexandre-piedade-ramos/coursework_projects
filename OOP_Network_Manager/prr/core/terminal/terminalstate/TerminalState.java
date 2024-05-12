package prr.core.terminal.terminalstate;

import prr.core.terminal.Terminal;

import java.io.Serializable;

import prr.core.exception.*;

public abstract class TerminalState implements Serializable{
    private static final long serialVersionUID = 202208091753L;
    private Terminal _terminal;
    


    public TerminalState(Terminal terminal){
        _terminal=terminal;
    }


    public abstract boolean StateToIdle() throws SameStateException;
    public abstract boolean StateToOff()throws SameStateException;
    public abstract boolean StateToSilent() throws SameStateException;
    public abstract boolean StateToBusy() throws SameStateException;
    public abstract String toString();
    public abstract boolean canStartCommunication();
    public abstract boolean canAcceptTextCommunication() throws  DestinationIsOffException;
    public abstract boolean canAcceptInteractiveCommunication()throws DestinationIsSilentException, DestinationIsOffException, DestinationIsBusyException;

    public Terminal getTerminal(){
        return _terminal;
    }
}

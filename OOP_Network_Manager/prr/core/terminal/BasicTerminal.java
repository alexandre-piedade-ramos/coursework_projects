package prr.core.terminal;

import prr.core.client.*;
import prr.core.exception.*;
import prr.core.communications.*;

import java.io.Serializable;

public class BasicTerminal extends Terminal implements Serializable{

    private static final long serialVersionUID = 202208091753L;

    public BasicTerminal(String id, Client owner){
        super(id, owner);
    }

    public boolean VideoCallCompatible(){
        return false;
    }

    public String getTypeName(){
        return TerminalType.BASIC.name();
    }
}
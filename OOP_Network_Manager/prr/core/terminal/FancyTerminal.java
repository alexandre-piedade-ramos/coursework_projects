package prr.core.terminal;

import prr.core.client.*;

import java.io.Serializable;

public class FancyTerminal extends Terminal implements Serializable{

    private static final long serialVersionUID = 202208091753L;

    public FancyTerminal(String id, Client owner){
        super(id, owner);
    }

    public boolean VideoCallCompatible(){
        return true;
    }

    public String getTypeName(){
        return TerminalType.FANCY.name();
    }
}
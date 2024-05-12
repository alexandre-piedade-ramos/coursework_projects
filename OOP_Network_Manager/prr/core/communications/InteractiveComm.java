package prr.core.communications;

import prr.core.terminal.*;
import java.io.Serializable;
public abstract class InteractiveComm extends Comm implements Serializable{

    private static final long serialVersionUID = 202208091753L;

    protected InteractiveComm(int id, Terminal from, Terminal to ){
        super(id, from, to);
    }

    public void endInteractiveComm(int durationMin){
        this.endComm();
        this.setLength(durationMin);

    }
}
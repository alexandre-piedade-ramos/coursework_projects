package prr.core.tariffplans;

import prr.core.client.clientlevels.*;
import prr.core.client.Client;
import prr.core.communications.*;
import java.io.Serializable;
public abstract class TariffPlan implements Serializable{

    private static final long serialVersionUID = 202208091753L;
    private String _name;

    protected TariffPlan(String name){
        _name=name;
    }

    public abstract double computeCommCost(String level, TextComm comm);
    public abstract double computeCommCost(String level, VoiceComm comm);
    public abstract double computeCommCost(String level, VideoComm comm);

}

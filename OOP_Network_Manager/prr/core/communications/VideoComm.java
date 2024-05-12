package prr.core.communications;

import prr.core.terminal.*;
import prr.core.client.Client;
import java.io.Serializable;
public class VideoComm extends InteractiveComm implements Serializable{

    private static final long serialVersionUID = 202208091753L;

    public VideoComm(int id, Terminal from, Terminal to ){
        super(id, from, to);
    }

    public double computeCost(){
       Client client=this.getFrom().getClient();
       double cost = client.getTariffPlan().computeCommCost(client.getLevel().toString(), this);
        if(this.getFrom().isFriend(this.getTo())){
            cost=cost*0.5;
        }
        this.setCost(cost);
        return cost;
    }

    public String getTypeName(){
        return CommNames.VIDEO.name();
    }
}

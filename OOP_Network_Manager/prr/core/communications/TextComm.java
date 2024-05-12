package prr.core.communications;

import prr.core.terminal.*;
import prr.core.client.*;
import java.io.Serializable;
public class TextComm extends Comm implements Serializable{

    private static final long serialVersionUID = 202208091753L;
    private String _text;

    public TextComm(int id, Terminal from, Terminal to, String text ){
        super(id, from, to);
        _text=text;
        this.setLength(text.length());
        this.computeCost();
        this.endComm();
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

    public int getLength(){
        return _text.length();
    }

    public String getTypeName(){
        return CommNames.TEXT.name();
    }


}

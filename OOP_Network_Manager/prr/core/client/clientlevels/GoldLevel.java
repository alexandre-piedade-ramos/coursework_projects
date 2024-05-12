package prr.core.client.clientlevels;

import java.io.Serializable;

import prr.core.client.Client;
import prr.core.communications.*;
public class GoldLevel extends ClientLevel implements Serializable{
    private static final long serialVersionUID = 202208091753L;

    public GoldLevel(Client client){
        super(client);
    }

    protected boolean LevelToNormal(){
        Client client = this.getClient();
        if(this.getClient().calcBalance()<0){
            client.setLevel(new NormalLevel(client));
            return true;
        }
        return false;
    }
    protected boolean LevelToGold(){
        return false;
    }
    protected boolean LeveltoPlatinum(){
        Client client = this.getClient();
        if(client.calcBalance()>0){
            for (Comm c:client.getLastComms(5)){
                if(!(c instanceof VideoComm)) return false;
            }
            client.setLevel(new GoldLevel(client));
            return true;
        }
        return false;
    }
    public boolean updateLevel(){
        if(this.LevelToNormal()) return true;
        if(this.LevelToGold()) return true;
        if(this.LeveltoPlatinum()) return true;

        return false;
    }
    
    public String toString(){
        return ClientLevelNames.NORMAL.name();
    }


}

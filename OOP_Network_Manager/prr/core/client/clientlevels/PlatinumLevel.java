package prr.core.client.clientlevels;

import java.io.Serializable;

import prr.core.client.Client;
import prr.core.communications.*;
public class PlatinumLevel extends ClientLevel implements Serializable{
    private static final long serialVersionUID = 202208091753L;

    public PlatinumLevel(Client client){
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
        Client client = this.getClient();
        if(client.calcBalance()>0){
            for (Comm c:client.getLastComms(5)){
                if(!(c instanceof TextComm)) return false;
            }
            client.setLevel(new GoldLevel(client));
            return true;
        }
        return false;
    }
    protected boolean LeveltoPlatinum(){   
        return false;
    }
    public boolean updateLevel(){
        if(this.LevelToNormal()) return true;
        if(this.LevelToGold()) return true;
        if(this.LeveltoPlatinum()) return true;

        return false;
    }
    
    public String toString(){
        return ClientLevelNames.PLATINUM.name();
    }
}
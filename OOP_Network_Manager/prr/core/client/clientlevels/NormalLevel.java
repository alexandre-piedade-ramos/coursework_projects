package prr.core.client.clientlevels;

import java.io.Serializable;

import prr.core.client.Client;

public class NormalLevel extends ClientLevel implements Serializable{
    private static final long serialVersionUID = 202208091753L;

    public NormalLevel(Client client){
        super(client);
    }
    
    protected boolean LevelToNormal(){
        return false;
    }
    protected boolean LevelToGold(){
        Client client = this.getClient();
        if(client.calcBalance()>500){
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
        return ClientLevelNames.NORMAL.name();
    }

}

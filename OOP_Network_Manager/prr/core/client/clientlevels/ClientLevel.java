package prr.core.client.clientlevels;

import java.io.Serializable;

import prr.core.client.Client;

public abstract class ClientLevel implements Serializable{
    private static final long serialVersionUID = 202208091753L;
    private Client _client;

    public ClientLevel(Client client){
        _client=client;
    }

    protected abstract boolean LevelToNormal();
    protected abstract boolean LevelToGold();
    protected abstract boolean LeveltoPlatinum();
    

    public abstract boolean updateLevel();
    
    public abstract String toString();

    public Client getClient(){
        return _client;
    }
}

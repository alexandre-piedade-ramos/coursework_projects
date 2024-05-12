package prr.core.notifications;

import prr.core.terminal.*;
import prr.core.client.Client;
import java.io.Serializable;
public class InteractiveCommNotification extends Notification implements Serializable{
    private static final long serialVersionUID = 202208091753L;
    public InteractiveCommNotification(Terminal sender, Terminal receiver){
        super(sender,receiver);
    }
    
    public void update(){
        boolean canSend=false;
        try{
            canSend=getSender().getState().canAcceptInteractiveCommunication();
        }catch(Exception exc){
            //expected and doesnt matter
            
        }finally{
            if(canSend){
                this.send();
            }
        }
    }
}
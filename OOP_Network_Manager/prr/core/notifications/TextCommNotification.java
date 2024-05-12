package prr.core.notifications;

import prr.core.terminal.*;
import java.io.Serializable;
public class TextCommNotification extends Notification implements Serializable{
    private static final long serialVersionUID = 202208091753L;
    public TextCommNotification(Terminal sender, Terminal receiver){
        super(sender,receiver);
    }
    
    public void update(){
        boolean canSend=false;
        try{
            canSend=getSender().getState().canAcceptTextCommunication();
        }catch(Exception exc){ //never matters here, and is expected
        }finally{
            if(canSend){
                this.send();
            }
        }
    }
}
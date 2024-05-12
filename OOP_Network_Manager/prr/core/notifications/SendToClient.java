package prr.core.notifications;

import prr.core.client.Client;
import java.io.Serializable;
public class SendToClient implements NotifDeliveryMethod,Serializable{
    private static final long serialVersionUID = 202208091753L;
    private Client _client;
    public SendToClient(Client c){
        _client=c;
    }
    public void sendNotif(Notification notif){
        _client.addNotif(notif);
    }
}
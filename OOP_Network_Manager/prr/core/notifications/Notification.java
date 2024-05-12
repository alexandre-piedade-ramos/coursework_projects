package prr.core.notifications;

import prr.core.terminal.*;
import java.io.Serializable;
public abstract class Notification implements TerminalStateObserver,Serializable{
    private String _type;
    private Terminal _sender;
    private Terminal _receiver;
    private boolean _stillObserving;
    private static final long serialVersionUID = 202208091753L;
    public Notification(Terminal sender, Terminal receiver){

        _sender=sender;
        _receiver=receiver;
        _stillObserving=true;
        _sender.addStateObserver(this);
    }
    @Override
    public boolean equals(Object other){
        if(!(other instanceof Notification)){
            return false;
        }
        Notification otherNotif=(Notification) other ;
        return otherNotif.getSender().getId().equals(_sender.getId()) &&
                    otherNotif.getType().equals(_type);
    }
    public void stopObserving(){
        _stillObserving=false;
    }
    public boolean stillObserving(){
        return _stillObserving;
    }
    public void setType(String type ){
        _type=type;
    }
    public String getType(){
        return _type;
    }
    public Terminal getSender(){
        return _sender;
    }
    public Terminal getReceiver(){
        return _receiver;
    }
    public void sendNotif(){
        _receiver.getClient().getDeliveryMethod().sendNotif(this);
    }
    @Override
    public String toString(){
        return _type+"|"+_sender.getId();
    }

    public void send(){
        this.setType(getSender().getPreviousState().toString().charAt(0)+"2"+   
                             getSender().getState().toString().charAt(0));
        this.getReceiver().getClient().getDeliveryMethod().sendNotif(this);
        this.stopObserving();
    }
}
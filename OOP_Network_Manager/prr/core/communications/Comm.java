package prr.core.communications;

import prr.core.terminal.Terminal;

import java.io.Serializable;
public abstract class Comm implements Serializable{

    private static final long serialVersionUID = 202208091753L;
    private int _id;
    private boolean _isPaid;
    private double _cost;
    private boolean _finished;
    private Terminal _from;
    private Terminal _to;
    private int _length;

    public Comm(int id, Terminal from, Terminal to ){
        _id=id;
        _from=from;
        _to=to;
    }
    
    public abstract double computeCost();

    
    public int getLength(){
        return _length;
    }

    public void setLength(int len){
        _length=len;
    }
    public double getCost(){
        return _cost;
    }
    public void setCost(double cost){
        _cost=cost;
    }

    public void endComm(){
        _finished=true;
    }

    public Terminal  getFrom(){
        return _from; 
    }

    public Terminal getTo(){
        return _to;
    }
    public abstract String getTypeName();
    @Override
    public String toString(){
        String status = _finished?"FINISHED":"ONGOING";
        
        return this.getTypeName()+"|" + _id +"|"+ _from.getId() +"|"+
                _to.getId() +"|" + _length + "|" + this.roundCostToLong() +
                 "|" + status;
    }

    public long roundCostToLong(){
        return (long) Math.round(_cost);
    }
    
    public boolean isPaid(){
        return _isPaid;
    }

    public double payComm(){
        _isPaid=true;
        _from.addPayment(_cost);
        _from.getClient().addPayment(_cost);

        return _cost;
    }
    public int getId(){
        return _id;
    }
    @Override
    public boolean equals(Object other){
        if(!(other instanceof Comm)) return false;

        Comm otherComm= (Comm) other;
        return otherComm.getId()==this._id;
    }

    
    public boolean isFinished(){
        return _finished;
    }
}

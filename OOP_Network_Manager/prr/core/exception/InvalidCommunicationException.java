package prr.core.exception;

public class InvalidCommunicationException extends Exception{
    
    private static final long serialVersionUID = 202208091753L;
    private int _key;

    public InvalidCommunicationException(int key){
        _key=key;
    }
    public int getKey(){ return _key;}
}
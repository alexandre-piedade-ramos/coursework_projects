package prr.core.exception;

public class DestinationIsBusyException extends Exception{

    private static final long serialVersionUID = 202208091753L;
    
    private String _key;

    public DestinationIsBusyException(String key){
        _key=key;
    }
    public String getKey(){ return _key;}
}
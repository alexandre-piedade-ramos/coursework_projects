package prr.core.exception;

public class UnsupportedAtOriginException extends Exception{

    private static final long serialVersionUID = 202208091753L;
    
    private String _key;
    private String _type;

    public UnsupportedAtOriginException(String key, String type){
        _key=key;
        _type=type;
    }
    public String getKey(){ return _key;}
    public String getType(){ return _type;}
}
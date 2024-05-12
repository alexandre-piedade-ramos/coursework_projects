package prr.core.exception;

public class UnknownTerminalKeyExceptionCore extends Exception{
    private static final long serialVersionUID = 202208091753L;
    private String _key;

    public UnknownTerminalKeyExceptionCore(String key){
        _key=key;
    }

    public String getKey(){
        return _key;
    }
}
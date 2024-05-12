package prr.app.terminals;

import prr.core.Network;
import prr.core.exception.*;
import prr.core.terminal.TerminalType;
import prr.app.exception.DuplicateTerminalKeyException;
import prr.app.exception.InvalidTerminalKeyException;
import prr.app.exception.UnknownClientKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

/**
 * Register terminal.
 */
class DoRegisterTerminal extends Command<Network> {

  DoRegisterTerminal(Network receiver) {
    super(Label.REGISTER_TERMINAL, receiver);
    addStringField("id", Message.terminalKey());
    addOptionField("type",Message.terminalType(), 
                    TerminalType.BASIC.toString(),TerminalType.FANCY.toString());
    addStringField("owner", Message.clientKey());
  }

  @Override
  protected final void execute() throws CommandException, 
      DuplicateTerminalKeyException, InvalidTerminalKeyException, 
          UnknownClientKeyException{

    String id = stringField("id");
    String type = stringField("type");
    String owner = stringField("owner");

    try{
      _receiver.registerTerminal(id, type, owner);
    }
    catch(DuplicateTerminalKeyExceptionCore exc){
      throw new DuplicateTerminalKeyException(exc.getKey());
    }
    catch(InvalidTerminalKeyExceptionCore exc){
      throw new InvalidTerminalKeyException(exc.getKey());
    }
    catch(UnknownClientKeyExceptionCore exc){
      throw new UnknownClientKeyException(exc.getKey());
    }

  }
}

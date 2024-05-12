package prr.app.terminal;

import prr.core.Network;
import prr.core.terminal.*;
import prr.app.exception.UnknownTerminalKeyException;
import prr.core.exception.UnknownTerminalKeyExceptionCore;


import pt.tecnico.uilib.menus.CommandException;

/**
 * Add a friend.
 */
class DoAddFriend extends TerminalCommand {

  DoAddFriend(Network context, Terminal terminal) {
    super(Label.ADD_FRIEND, context, terminal);
    //FIXME add command fields
    addStringField("id", Message.terminalKey());

  }
  
  @Override
  protected final void execute() throws CommandException,
                                        UnknownTerminalKeyException {
    String id= stringField("id");

    try{
      _network.addFriend(_receiver.getId() , id);
    }
    catch(UnknownTerminalKeyExceptionCore exc){
      throw new UnknownTerminalKeyException(exc.getKey());
    }

     
  }
}

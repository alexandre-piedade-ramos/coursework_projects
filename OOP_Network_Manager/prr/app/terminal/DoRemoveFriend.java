package prr.app.terminal;

import prr.core.Network;
import prr.core.terminal.*;
import pt.tecnico.uilib.menus.CommandException;

import prr.app.exception.UnknownTerminalKeyException;
import prr.core.exception.UnknownTerminalKeyExceptionCore;

/**
 * Remove friend.
 */
class DoRemoveFriend extends TerminalCommand {

  DoRemoveFriend(Network context, Terminal terminal) {
    super(Label.REMOVE_FRIEND, context, terminal);
    addStringField("id", Message.terminalKey());
  }
  
  @Override
  protected final void execute() throws CommandException,
                                        UnknownTerminalKeyException {
    String id= stringField("id");
    try{
      _network.rmvFriend(_receiver.getId() , id);
    }
    catch(UnknownTerminalKeyExceptionCore exc){
      throw new UnknownTerminalKeyException(exc.getKey());
    }
  }
}

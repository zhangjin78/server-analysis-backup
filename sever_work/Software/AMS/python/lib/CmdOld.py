import os
import commands

class CmdOld:
    def __init__(self, logger = None, dry = False):
        self.logger = logger
        self.dry = dry

    def cmd(self, cmd):
        if self.logger == None:
            print cmd
            if self.dry == False:
                res = os.system(cmd)
            else:
                res = 0
        else:
            self.logger.info("CMD='%s'", cmd)
            if self.dry == False:
                res = os.system(cmd)
            else:
                res = 0

            self.logger.info("CMD_EXIT_CODE=%d", res)

        return res


    def cmd_output(self, cmd):
        if self.logger == None:
            print cmd
            if self.dry == False:
                res = commands.getstatusoutput(cmd)
            else:
                res = [0,]
        else:
            self.logger.info("CMD='%s'", cmd)
            if self.dry == False:
                res = commands.getstatusoutput(cmd)
            else:
                res = [0, "123",]

            self.logger.info("CMD_EXIT_CODE=%d", res[0])

        return res

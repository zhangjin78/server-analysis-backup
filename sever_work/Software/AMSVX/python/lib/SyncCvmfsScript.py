import os
import logging
import logging.handlers
import traceback
import argparse
import ConfigParser
import Logger
import CmdOld

APP_NAME = 'sync_cvmfs'
LOG_DIR = '/tmp/'

CMD_CVMFS_SERVER = 'cvmfs_server'
CMD_RSYNC = '/usr/bin/rsync'
CMD_TOUCH = '/bin/touch'
CMD_STAT = '/usr/bin/stat'
CMD_DATE = '/bin/date'
BASE_DESTINATION = '/cvmfs/ams.cern.ch/Offline'
BASE_SOURCE = '/afs/cern.ch/ams/Offline'

class SyncCvmfsScriptException(Exception):
    def __init__(self, error):
        self.error = error
    def __str__(self):
        return self.error.encode("utf-8")

class InfoSyncCvmfsScriptException(SyncCvmfsScriptException):
    pass
    
class WarningSyncCvmfsScriptException(SyncCvmfsScriptException):
    pass

class ErrorSyncCvmfsScriptException(SyncCvmfsScriptException):
    pass

class SyncCvmfsScript:
    execute = False
    folders = []
    folders_change_time = []

    def __init__(self):
        parser = argparse.ArgumentParser(description='Sync cvmfs script.')
        parser.add_argument('-e', '--execute', help='Execute shell command', action='store_true')
        parser.add_argument('-f', '--folders', help='Directory for sync', nargs='*')
        parser.add_argument('-c', '--config',  help='Config file')
        args = parser.parse_args()
        if args.execute:
            self.execute = True

        if args.folders:
            self.folders = args.folders
        else:
            if args.config:
                config_file = args.config
            else:
                config_file = os.path.realpath("%s/../../%s" % (__file__, 'sync_cvmfs.conf'))

            if not os.path.isfile(config_file):
                raise ErrorSyncCvmfsScriptException("Config file not found")

            config = ConfigParser.RawConfigParser()
            config.read(config_file)
            self.folders = self._readConfigSection(config, 'folders')
            self.folders_change_time = self._readConfigSection(config, 'folders_change_time')

        self.logger = Logger.Logger(APP_NAME, log_dir = LOG_DIR, log_file_with_date = False).getLogger(level_log=logging.INFO, level_console=logging.INFO)
        self.cmd = CmdOld.CmdOld(logger=self.logger, dry=(not self.execute))

    def run(self):
        self.logger.info("SyncCvmfsScript.run start")
        if self.execute == False:
            self.logger.info("Script running in dry-mode, without real execute the commands. Specify --execute parameter for doing the job.")

        try:
            self.__startTransaction()

            for folder in self.folders:
                self.__sync(folder)
                self.__touch(folder)

            for folder in self.folders_change_time:
                self.__sync(folder)
                self.__touch_and_change_mtime(folder)

            # do publish manually
            #self.__finishTransaction()

        except(InfoSyncCvmfsScriptException), error:
            self.logger.info(error)
        except(WarningSyncCvmfsScriptException), error:
            self.logger.warning(error)
        except(ErrorSyncCvmfsScriptException), error:
            self.logger.error(error)

        res = self.logger.info("SyncCvmfsScript.run finish")


    def _readConfigSection(self, config, section):
        lst = config.get(APP_NAME, section).lstrip().rstrip().split('\n')
        # remove comments lines and strip last comma
        lst = [line.rstrip(',') for line in lst]
        return filter(self._removeComments, lst)


    def _removeComments(self, line):
        return (line[0] != '#')


    def __startTransaction(self):
        res = self.cmd.cmd("%s transaction" % CMD_CVMFS_SERVER)
        if (res != 0):
            raise ErrorSyncCvmfsScriptException("Start cvmfs transaction failed")


    def __finishTransaction(self):
        res = self.cmd.cmd("%s publish" % CMD_CVMFS_SERVER)
        if (res != 0):
            raise ErrorSyncCvmfsScriptException("Publish cvmfs transaction failed")


    def __sync(self, folder):
        res = self.cmd.cmd("%s -av --delete-during -e \"ssh -l ams\" ams@lxplus:%s/%s/ %s/%s" % (CMD_RSYNC, BASE_SOURCE, folder, BASE_DESTINATION, folder))
        if res != 0:
            raise ErrorSyncCvmfsScriptException("Sync directory <%s/%s> failed" % (BASE_SOURCE, folder))


    def __touch(self, folder, mtime = None):
        if mtime is None:
            res = self.cmd.cmd("%s %s/%s/.cvmfscatalog" % (CMD_TOUCH, BASE_DESTINATION, folder))
        else:
            res = self.cmd.cmd("%s -d \"%s\" %s/%s/.cvmfscatalog" % (CMD_TOUCH, mtime, BASE_DESTINATION, folder))

        if res != 0:
            raise ErrorSyncCvmfsScriptException("Create file <%s/%s> failed" % (BASE_DESTINATION, folder))


    def __touch_and_change_mtime(self, folder):
        old_ts = self.cmd.cmd_output('%s -c%%Y %s/%s' % (CMD_STAT, BASE_DESTINATION, folder))
        old_mtime = self.cmd.cmd_output('%s --date="@%s"' % (CMD_DATE, old_ts[1]))
        self.__touch(folder, old_mtime[1])

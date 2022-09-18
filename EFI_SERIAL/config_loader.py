# config_loader.py
from configparser import SafeConfigParser
import os


class Config:
    """Interact with configuration variables."""

    configParser = SafeConfigParser()
    configFilePath = (os.path.join(os.getcwd(), 'config.ini'))
    configParser.read(configFilePath)

    @classmethod
    def gen(cls, key):
        """Get gen values from config.ini."""
        return cls.configParser.get('GEN', key)

#include "gameskyrimsegog.h"

#include "skyrimsedataarchives.h"
#include "skyrimsescriptextender.h"
#include "skyrimseunmanagedmods.h"
#include "skyrimsemoddatachecker.h"
#include "skyrimsemoddatacontent.h"
#include "skyrimsesavegame.h"

#include <pluginsetting.h>
#include <executableinfo.h>
#include <gamebryosavegameinfo.h>
#include <gamebryolocalsavegames.h>
#include <creationgameplugins.h>
#include "versioninfo.h"
#include <utility.h>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

#include <memory>
#include "scopeguard.h"

using namespace MOBase;

GameSkyrimSEGOG::GameSkyrimSEGOG()
{
}

void GameSkyrimSEGOG::setGamePath(const QString &path)
{
    m_GamePath = path;
}

QDir GameSkyrimSEGOG::documentsDirectory() const
{
    return m_MyGamesPath;
}

QString GameSkyrimSEGOG::identifyGamePath() const
{
    QString path = "Software\\Bethesda Softworks\\" + gameName();
    return findInRegistry(HKEY_LOCAL_MACHINE, path.toStdWString().c_str(), L"Installed Path");
}

QDir GameSkyrimSEGOG::savesDirectory() const
{
    return QDir(m_MyGamesPath + "/Saves");
}

QString GameSkyrimSEGOG::myGamesPath() const
{
    return m_MyGamesPath;
}

bool GameSkyrimSEGOG::isInstalled() const
{
    return !m_GamePath.isEmpty();
}

bool GameSkyrimSEGOG::init(IOrganizer *moInfo)
{
    if (!GameGamebryo::init(moInfo)) {
        return false;
    }

    registerFeature<ScriptExtender>(new SkyrimSEScriptExtender(this));
    registerFeature<DataArchives>(new SkyrimSEDataArchives(myGamesPath()));
    registerFeature<LocalSavegames>(new GamebryoLocalSavegames(myGamesPath(), "Skyrimcustom.ini"));
    registerFeature<ModDataChecker>(new SkyrimSEModDataChecker(this));
    registerFeature<ModDataContent>(new SkyrimSEModDataContent(this));
    registerFeature<SaveGameInfo>(new GamebryoSaveGameInfo(this));
    registerFeature<GamePlugins>(new CreationGamePlugins(moInfo));
    registerFeature<UnmanagedMods>(new SkyrimSEUnmangedMods(this));

    return true;
}



QString GameSkyrimSEGOG::gameName() const
{
    return "Skyrim Special Edition GOG";
}

QList<ExecutableInfo> GameSkyrimSEGOG::executables() const
{
    return QList<ExecutableInfo>()
        << ExecutableInfo("SKSE", findInGameFolder(feature<ScriptExtender>()->loaderName()))
        << ExecutableInfo("Skyrim Special Edition", findInGameFolder(binaryName()))
        << ExecutableInfo("Skyrim Special Edition Launcher", findInGameFolder(getLauncherName()))
        << ExecutableInfo("Creation Kit", findInGameFolder("CreationKit.exe"))
        << ExecutableInfo("LOOT", getLootPath()).withArgument("--game=\"Skyrim Special Edition\"")
        ;
}

QList<ExecutableForcedLoadSetting> GameSkyrimSEGOG::executableForcedLoads() const
{
    return QList<ExecutableForcedLoadSetting>();
}

QFileInfo GameSkyrimSEGOG::findInGameFolder(const QString &relativePath) const
{
    return QFileInfo(m_GamePath + "/" + relativePath);
}

QString GameSkyrimSEGOG::name() const
{
    return "Skyrim Special Edition GOG Support Plugin";
}

QString GameSkyrimSEGOG::localizedName() const
{
  return tr("Skyrim Special Edition GOG Support Plugin");
}

QString GameSkyrimSEGOG::author() const
{
    return "Archost & ZachHaber & KAOS";
}

QString GameSkyrimSEGOG::description() const
{
    return tr("Adds support for the game Skyrim Special Edition GOG.");
}

MOBase::VersionInfo GameSkyrimSEGOG::version() const
{
    return VersionInfo(1, 6, 0, VersionInfo::RELEASE_FINAL);
}

QList<PluginSetting> GameSkyrimSEGOG::settings() const
{
  return {
    PluginSetting("enderal_downloads", "allow Enderal and Enderal SE downloads", QVariant(false))
  };
}

void GameSkyrimSEGOG::initializeProfile(const QDir &path, ProfileSettings settings) const
{
    if (settings.testFlag(IPluginGame::MODS)) {
        copyToProfile(localAppFolder() + "/Skyrim Special Edition GOG", path, "plugins.txt");
    }

    if (settings.testFlag(IPluginGame::CONFIGURATION)) {
        if (settings.testFlag(IPluginGame::PREFER_DEFAULTS)
            || !QFileInfo(myGamesPath() + "/skyrim.ini").exists()) {
            copyToProfile(gameDirectory().absolutePath(), path, "skyrim_default.ini", "skyrim.ini");
        }
        else {
            copyToProfile(myGamesPath(), path, "skyrim.ini");
        }

        copyToProfile(myGamesPath(), path, "skyrimprefs.ini");
        copyToProfile(myGamesPath(), path, "skyrimcustom.ini");
    }
}

QString GameSkyrimSEGOG::savegameExtension() const
{
    return "ess";
}

QString GameSkyrimSEGOG::savegameSEExtension() const
{
    return "skse";
}

std::shared_ptr<const GamebryoSaveGame> GameSkyrimSEGOG::makeSaveGame(QString filePath) const
{
  return std::make_shared<const SkyrimSESaveGame>(filePath, this);
}

QString GameSkyrimSEGOG::steamAPPId() const
{
    return "489830";
}

QStringList GameSkyrimSEGOG::primaryPlugins() const
{
    QStringList plugins = {
      "skyrim.esm",
      "update.esm",
      "dawnguard.esm",
      "hearthfires.esm",
      "dragonborn.esm"
    };

    plugins.append(CCPlugins());

    return plugins;
}

QStringList GameSkyrimSEGOG::gameVariants() const
{
    return{ "Regular"};
}

QString GameSkyrimSEGOG::gameShortName() const
{
    return "SkyrimSE";
}

QStringList GameSkyrimSEGOG::validShortNames() const
{
  QStringList shortNames{ "Skyrim" };
  if (m_Organizer->pluginSetting(name(), "enderal_downloads").toBool()) {
    shortNames.append({ "Enderal", "EnderalSE" });
  }
  return shortNames;
}

QString GameSkyrimSEGOG::gameNexusName() const
{
    return "skyrimspecialedition";
}

QStringList GameSkyrimSEGOG::iniFiles() const
{
    return{ "skyrim.ini", "skyrimprefs.ini", "skyrimcustom.ini" };
}

QStringList GameSkyrimSEGOG::DLCPlugins() const
{
    return{ "dawnguard.esm", "hearthfires.esm", "dragonborn.esm" };
}

QStringList GameSkyrimSEGOG::CCPlugins() const
{
  QStringList plugins;
  std::set<QString> pluginsLookup;

  const QString path = gameDirectory().filePath("Skyrim.ccc");

  MOBase::forEachLineInFile(path, [&](QString s) {
    const auto lc = s.toLower();
    if (!pluginsLookup.contains(lc)) {
      pluginsLookup.insert(lc);
      plugins.append(std::move(s));
    }
  });

  return plugins;
}

IPluginGame::LoadOrderMechanism GameSkyrimSEGOG::loadOrderMechanism() const
{
    return IPluginGame::LoadOrderMechanism::PluginsTxt;
}

int GameSkyrimSEGOG::nexusModOrganizerID() const
{
    return 6194; //... Should be 0?
}

int GameSkyrimSEGOG::nexusGameID() const
{
    return 1704; //1704
}

QDir GameSkyrimSEGOG::gameDirectory() const
{
    return QDir(m_GamePath);
}

// Not to delete all the spaces...
MappingType GameSkyrimSEGOG::mappings() const
{
    MappingType result;

    for (const QString &profileFile : { "plugins.txt", "loadorder.txt" }) {
        result.push_back({ m_Organizer->profilePath() + "/" + profileFile,
            localAppFolder() + "/" + gameName() + "/" + profileFile,
            false });
    }

    return result;
}

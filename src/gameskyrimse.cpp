#include "gameskyrimse.h"

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

GameSkyrimSE::GameSkyrimSE()
{
}

void GameSkyrimSE::setGamePath(const QString &path)
{
    m_GamePath = path;
}

QDir GameSkyrimSE::documentsDirectory() const
{
    return m_MyGamesPath;
}

QString GameSkyrimSE::identifyGamePath() const
{
    QString path = "Software\\Bethesda Softworks\\" + gameName();
    return findInRegistry(HKEY_LOCAL_MACHINE, path.toStdWString().c_str(), L"Installed Path");
}

QDir GameSkyrimSE::savesDirectory() const
{
    return QDir(m_MyGamesPath + "/Saves");
}

QString GameSkyrimSE::myGamesPath() const
{
    return m_MyGamesPath;
}

bool GameSkyrimSE::isInstalled() const
{
    return !m_GamePath.isEmpty();
}

bool GameSkyrimSE::init(IOrganizer *moInfo)
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



QString GameSkyrimSE::gameName() const
{
    return "Skyrim Special Edition";
}

QList<ExecutableInfo> GameSkyrimSE::executables() const
{
    return QList<ExecutableInfo>()
        << ExecutableInfo("SKSE", findInGameFolder(feature<ScriptExtender>()->loaderName()))
        << ExecutableInfo("Skyrim Special Edition", findInGameFolder(binaryName()))
        << ExecutableInfo("Skyrim Special Edition Launcher", findInGameFolder(getLauncherName()))
        << ExecutableInfo("Creation Kit", findInGameFolder("CreationKit.exe"))
        << ExecutableInfo("LOOT", getLootPath()).withArgument("--game=\"Skyrim Special Edition\"")
        ;
}

QList<ExecutableForcedLoadSetting> GameSkyrimSE::executableForcedLoads() const
{
    return QList<ExecutableForcedLoadSetting>();
}

QFileInfo GameSkyrimSE::findInGameFolder(const QString &relativePath) const
{
    return QFileInfo(m_GamePath + "/" + relativePath);
}

QString GameSkyrimSE::name() const
{
    return "Skyrim Special Edition Support Plugin";
}

QString GameSkyrimSE::localizedName() const
{
  return tr("Skyrim Special Edition Support Plugin");
}

QString GameSkyrimSE::author() const
{
    return "Archost & ZachHaber";
}

QString GameSkyrimSE::description() const
{
    return tr("Adds support for the game Skyrim Special Edition.");
}

MOBase::VersionInfo GameSkyrimSE::version() const
{
    return VersionInfo(1, 6, 0, VersionInfo::RELEASE_FINAL);
}

QList<PluginSetting> GameSkyrimSE::settings() const
{
  return {
    PluginSetting("enderal_downloads", "allow Enderal and Enderal SE downloads", QVariant(false))
  };
}

void GameSkyrimSE::initializeProfile(const QDir &path, ProfileSettings settings) const
{
    if (settings.testFlag(IPluginGame::MODS)) {
        copyToProfile(localAppFolder() + "/Skyrim Special Edition", path, "plugins.txt");
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

QString GameSkyrimSE::savegameExtension() const
{
    return "ess";
}

QString GameSkyrimSE::savegameSEExtension() const
{
    return "skse";
}

std::shared_ptr<const GamebryoSaveGame> GameSkyrimSE::makeSaveGame(QString filePath) const
{
  return std::make_shared<const SkyrimSESaveGame>(filePath, this);
}

QString GameSkyrimSE::steamAPPId() const
{
    return "489830";
}

QStringList GameSkyrimSE::primaryPlugins() const
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

QStringList GameSkyrimSE::gameVariants() const
{
    return{ "Regular" };
}

QString GameSkyrimSE::gameShortName() const
{
    return "SkyrimSE";
}

QStringList GameSkyrimSE::validShortNames() const
{
  QStringList shortNames{ "Skyrim" };
  if (m_Organizer->pluginSetting(name(), "enderal_downloads").toBool()) {
    shortNames.append({ "Enderal", "EnderalSE" });
  }
  return shortNames;
}

QString GameSkyrimSE::gameNexusName() const
{
    return "skyrimspecialedition";
}

QStringList GameSkyrimSE::iniFiles() const
{
    return{ "skyrim.ini", "skyrimprefs.ini", "skyrimcustom.ini" };
}

QStringList GameSkyrimSE::DLCPlugins() const
{
    return{ "dawnguard.esm", "hearthfires.esm", "dragonborn.esm" };
}

QStringList GameSkyrimSE::CCPlugins() const
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

IPluginGame::LoadOrderMechanism GameSkyrimSE::loadOrderMechanism() const
{
    return IPluginGame::LoadOrderMechanism::PluginsTxt;
}

int GameSkyrimSE::nexusModOrganizerID() const
{
    return 6194; //... Should be 0?
}

int GameSkyrimSE::nexusGameID() const
{
    return 1704; //1704
}

QDir GameSkyrimSE::gameDirectory() const
{
    return QDir(m_GamePath);
}

// Not to delete all the spaces...
MappingType GameSkyrimSE::mappings() const
{
    MappingType result;

    for (const QString &profileFile : { "plugins.txt", "loadorder.txt" }) {
        result.push_back({ m_Organizer->profilePath() + "/" + profileFile,
            localAppFolder() + "/" + gameName() + "/" + profileFile,
            false });
    }

    return result;
}

#ifndef _SKYRIMSESAVEGAME_H
#define _SKYRIMSESAVEGAME_H

#include "gamebryosavegame.h"
#include "gameskyrimsegog.h"

namespace MOBase { class IPluginGame; }

class SkyrimSESaveGame : public GamebryoSaveGame
{
public:
  SkyrimSESaveGame(QString const &fileName, GameSkyrimSEGOG const *game);

protected:

  // Fetch easy-to-access information.
  void fetchInformationFields(FileWrapper& wrapper,
    unsigned long& version,
    QString& playerName,
    unsigned short& playerLevel,
    QString& playerLocation,
    unsigned long& saveNumber,
    FILETIME& creationTime) const;

  std::unique_ptr<DataFields> fetchDataFields() const override;

};

#endif // _SKYRIMSESAVEGAME_H

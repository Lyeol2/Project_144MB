#include "EditorBuildExporter.h"
#include <fstream>

bool EditorBuildExporter::ExportSceneForGame(const std::shared_ptr<EditorScene>& scene, const std::string& outPath) const
{
    if (!scene) return false;

    // TODO: 향후 바이너리 포맷이나 최적화된 형식으로 파일 쓰기 로직 구현
    // 현재는 단순 텍스트 시뮬레이션
    std::ofstream outFile(outPath);
    if (!outFile.is_open()) return false;

    outFile << "GAME_READY_LEVEL_DATA\n";
    outFile << "ObjectCount:" << scene->GetGameObjects().size() << "\n";
    // 실제로는 에디터 전용 메타데이터(선택 상태, 기즈모 등)를 제외하고 출력해야 함
    
    outFile.close();
    return true;
}

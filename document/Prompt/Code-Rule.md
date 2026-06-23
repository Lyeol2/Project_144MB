# 프로젝트명: 곡예 (가제) - 코딩 스타일 및 주석 규칙 (C++ 기준)

이 문서는 프로젝트 '곡예'의 코드 가독성 향상과 원활한 협업을 위해 모든 C++ 프로그래머(및 AI 어시스턴트)가 준수해야 할 네이밍 컨벤션, 코드 스타일 및 주석 작성 규칙을 정의합니다.

---

## 1. 네이밍 컨벤션 및 코드 스타일 (Naming Conventions & Style)
모던 C++(C++11 이상)의 범용적인 코딩 표준을 기본으로 따르며, 프로젝트 내 일관성을 유지합니다.

### 1.1 기본 네이밍 규칙
- **클래스, 구조체, 열거형(Enum):** `PascalCase`
  - 예: `PlayerController`, `GameState`, `BalanceData`
- **인터페이스 (순수 가상 클래스):** 대문자 `I` + `PascalCase`
  - 예: `IObstacle`, `IInteractable`
- **퍼블릭(Public) 멤버 변수:** `PascalCase`
  - 예: `MaxJumpDuration`, `IsBalancing`, `CurrentScore`
- **프라이빗(Private) 및 프로텍티드(Protected) 멤버 변수:** 접두사 `m_` + `camelCase`
  - 예: `m_currentBalance`, `m_rigidBody`, `m_isAirborne`
- **메서드(Method):** `PascalCase` (동사로 시작)
  - 예: `CalculateBalance()`, `ApplyWindForce()`
- **로컬 변수 및 매개변수(Parameter):** `camelCase`
  - 예: `targetPosition`, `deltaTime`, `windForce`
- **상수(const, constexpr) 및 매크로:** `UPPER_SNAKE_CASE`
  - 예: `MAX_TENSION_LIMIT`, `DEFAULT_GRAVITY`

### 1.2 서식 및 스타일 규칙
- **헤더 가드:** 모든 헤더 파일(`.h` 또는 `.hpp`)의 최상단에는 `#pragma once`를 사용합니다.
- **포인터와 참조자:** 타입 이름에 붙여 씁니다.
  - `Player* player` (O) / `Player *player` (X)
- **중괄호:** 항상 새로운 줄에서 시작합니다 (Allman 스타일).
- **들여쓰기:** 탭(Tab) 대신 스페이스(Space) 4칸을 사용하여 들여쓰기를 통일합니다.
- 조건문(`if`, `while`, `for`) 내부 코드가 한 줄이더라도 반드시 중괄호 `{}`를 사용합니다.
  ```cpp
  // Bad
  if (isDead) return;
  
  // Good
  if (isDead)
  {
      return;
  }
  ```

---

## 2. 주석 규칙 (Comment Rules)

### 2.1 주석 작성 원칙
- 코드가 *어떻게(How)* 동작하는지보다, **왜(Why)** 그렇게 작성되었는지를 설명하는 데 집중합니다.
- 명확하고 직관적인 변수명/함수명을 통해 불필요한 주석을 최소화하는 것을 우선합니다.

### 2.2 클래스 및 퍼블릭 메서드 주석 (Doxygen 스타일)
- 헤더 파일에 선언되는 클래스, 인터페이스, 퍼블릭 메서드 등에는 Doxygen 스타일(`///` 또는 `/** ... */`)의 주석을 작성합니다.
- 매개변수(`@param`)와 반환값(`@return`)의 의미를 명확히 기재해야 합니다.
  ```cpp
  /// @brief 입력된 힘에 따라 캐릭터의 밸런스 수치를 보정합니다.
  /// @param force 방향키 입력이나 바람에 의해 발생한 보정치
  /// @return 보정 후 밸런스가 한계에 도달해 추락했는지 여부
  bool CalculateBalance(float force);
  ```

### 2.3 인라인 주석 (Inline Comments)
- 복잡한 물리 수식, 메모리 관리(포인터 제어), 예외 처리 등 즉각적인 이해가 어려운 로직에만 `//` 를 사용하여 소스 파일(`.cpp`)에 작성합니다.
- 코드의 우측 끝에 다는 주석은 지양하고, 설명하려는 코드의 바로 **윗줄**에 작성합니다.

### 2.4 특수 주석 태그
작업 상태나 주의 사항을 팀원(또는 미래의 자신)에게 남길 때는 아래 태그를 접두사로 사용합니다.
- `// TODO: [이름/날짜] 나중에 추가하거나 수정해야 할 기능`
- `// FIXME: [이름/날짜] 수정이 필요한 버그나 로직의 결함 (메모리 누수 의심 등)`
- `// HACK: [이름/날짜] 올바른 구조는 아니지만 임시방편으로 해결한 코드 (리팩토링 요망)`

## 3. 엔진 개발 규칙 (Comment Rules)

### 3.1 런타임 및 에디터 코드 분리
- 런타임에서 빌드될시 사용될 코드와 에디터 코드는 분리되어있어야합니다
- 에디터 관련 클래스나 명명법은 항상 **Editor** 접두사를 사용합니다
	-  ex) EditorScene, EditorObject ... 등등

### 3.2 1.44MB 를 위한 최적화
- 런타임 빌드시 1.44MB 의 용량의 게임이 되어야합니다. 런타임에서는 라이브러리 활용을 최소로 하도록합니다.
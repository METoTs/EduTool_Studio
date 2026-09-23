# 전체 TC 실행 기록 — 2026-09-23

검증 코드: `bce57e9ab` (EduTool_Studio). 실행 빌드: build_vs2022/rundir/RelWithDebInfo/bin/64bit/obs64.exe. Windows 실제 UI를 Computer Use로 조작했다.

## 판정 기준

통과는 해당 기대 결과를 확인한 경우만 집계한다. 부분 확인·차단·미실행은 통과율에 포함하지 않는다. 기존 통과는 이전 증거의 재사용이며 이번 실행과 구분한다. 메인 공통 설정 콘텐츠는 제외하고 기능별 설정은 포함한다.

현재 집계: {"실패":6,"미실행":25,"부분 확인":19,"통과":9,"기존 통과":4,"차단":34} / 총 97건.

[참고 이미지 24장과 실제 화면 1:1 비교](edutool-ui-comparison-20260923.md). [실행 증거 폴더](../tc-screenshots/full-tc-20260923/). 스크린샷·영상은 gitignore 대상이며 문서만 커밋한다.

| TC | 절차 | 판정 | 실제 결과 / 한계 | 증거 |
|---|---|---|---|---|
| TC-001 | 앱 실행 후 녹화·라이브·편집·업로드를 차례로 선택 | 실패 | 재시작 후 편집 화면에 OBS 도크가 겹쳐 표시됨. 모드 전환 후 제어 도크가 사라져 독 메뉴에서 수동 복구해야 녹화 시작 가능. | [startup.png](../tc-screenshots/full-tc-20260923/startup.png)<br>[TC-001-record.png](../tc-screenshots/full-tc-20260923/TC-001-record.png)<br>[dock-controls-recovered.png](../tc-screenshots/full-tc-20260923/dock-controls-recovered.png) |
| TC-002 | 메인화면의 메뉴·버튼 확인 | 미실행 | 실행 대기 |  |
| TC-003 | 창 최소 크기와 일반 크기에서 주요 화면 확인 | 실패 | 편집 화면의 주요 버튼이 가로로 잘림. 재시작 시 기존 OBS 도크가 편집 영역을 차지함. | [startup.png](../tc-screenshots/full-tc-20260923/startup.png)<br>[TC-007-recording-edit.png](../tc-screenshots/full-tc-20260923/TC-007-recording-edit.png) |
| TC-004 | 앱 재실행 | 부분 확인 | 마지막 기능·녹화 경로는 재실행 시 유지. 계정 유지 정책은 시험 계정 부재로 미확인. 화면 복원 결함은 TC-001 참조. | [startup.png](../tc-screenshots/full-tc-20260923/startup.png) |
| TC-005 | 카메라·마이크·PC 소리를 연결/분리 | 부분 확인 | 실제 C920 연결·영상 확인. 장치 물리 분리/재연결은 미수행. 마이크 초기화 오류 로그가 있으나 연결됨으로 표시되어 정상 입력 여부 추가 확인 필요. | [TC-112-camera-added.png](../tc-screenshots/full-tc-20260923/TC-112-camera-added.png) |
| TC-006 | 기능 처리 중 오류 발생 | 통과 | 선택 시간 역전 및 미설정 방송 시작 시 원인 안내와 설정/취소 동작 확인. | [TC-512-error.png](../tc-screenshots/full-tc-20260923/TC-512-error.png)<br>[TC-406-unconfigured.png](../tc-screenshots/full-tc-20260923/TC-406-unconfigured.png) |
| TC-007 | 긴 작업 중 다른 기능 화면으로 이동 후 복귀 | 부분 확인 | 녹화 중 편집·라이브 전환/복귀해도 REC 시간과 파일 기록 유지. 방송·업로드 진행 중 전환은 연결 환경 부재로 미확인. | [TC-007-recording-edit.png](../tc-screenshots/full-tc-20260923/TC-007-recording-edit.png)<br>[TC-007-return.png](../tc-screenshots/full-tc-20260923/TC-007-return.png)<br>[TC-404-live-shared-camera.png](../tc-screenshots/full-tc-20260923/TC-404-live-shared-camera.png) |
| TC-101 | 녹화 화면 진입 | 기존 통과 | 이전 실행에서 통과한 항목. docs/edutool-progress.md 및 1:1 비교 자료 참조. 이번 전체 점검에서 모든 절차를 재수행한 것은 아님. |  |
| TC-102 | 프리뷰 전체화면 진입 후 종료 | 통과 | 녹화 중 전체화면 진입 및 Escape 복귀 후 REC 시간 증가·녹화 유지. | [TC-102-fullscreen.png](../tc-screenshots/full-tc-20260923/TC-102-fullscreen.png)<br>[TC-102-return.png](../tc-screenshots/full-tc-20260923/TC-102-return.png) |
| TC-103 | 우측 상태 패널 접기/펴기 | 통과 | 상태 패널 접기/펴기 후 미리보기 영역 변경 및 상태 유지 확인. | [TC-103-panel-open.png](../tc-screenshots/full-tc-20260923/TC-103-panel-open.png) |
| TC-104 | 구도 조정 진입 후 소스 위치 변경 | 기존 통과 | 이전 실행에서 통과한 항목. docs/edutool-progress.md 및 1:1 비교 자료 참조. 이번 전체 점검에서 모든 절차를 재수행한 것은 아님. |  |
| TC-105 | 녹화 폴더·품질·형식 선택 후 저장, 재진입 | 기존 통과 | 이전 실행에서 통과한 항목. docs/edutool-progress.md 및 1:1 비교 자료 참조. 이번 전체 점검에서 모든 절차를 재수행한 것은 아님. |  |
| TC-106 | 녹화 설정 변경 후 취소 | 기존 통과 | 이전 실행에서 통과한 항목. docs/edutool-progress.md 및 1:1 비교 자료 참조. 이번 전체 점검에서 모든 절차를 재수행한 것은 아님. |  |
| TC-107 | 기능별 녹화 설정을 기본값으로 초기화 | 미실행 | 실행 대기 |  |
| TC-108 | 쓰기 불가 폴더를 저장 위치로 지정하고 녹화 시작 | 미실행 | 실행 대기 |  |
| TC-109 | 디스크 공간 부족 상태에서 녹화 시작/진행 | 차단 | 저용량 시험 볼륨 없음. 운영 드라이브를 채우는 방법은 사용하지 않음. 공간 부족 시작/진행 시나리오 미실행. |  |
| TC-110 | 카메라·화면·미디어·이미지·텍스트·브라우저 소스를 각각 추가 | 부분 확인 | 소스 유형 목록·이미지·실제 C920 카메라 추가/속성 확인. 화면/미디어/텍스트/브라우저는 미확인. 카메라 추가 후 소스 관리 창이 속성 창을 가리는 모달 순서 결함 발견. | [TC-110-types.png](../tc-screenshots/full-tc-20260923/TC-110-types.png)<br>[TC-110-camera-properties.png](../tc-screenshots/full-tc-20260923/TC-110-camera-properties.png)<br>[TC-112-new-camera-pip.png](../tc-screenshots/full-tc-20260923/TC-112-new-camera-pip.png) |
| TC-111 | 소스 표시/숨김과 순서 변경 | 부분 확인 | 소스 아래로 이동 시 이미지가 카메라 위로 표시되고, 카메라 체크 해제 시 프리뷰에서 사라짐. 변경 상태의 녹화 결과는 미확인. | [TC-111-order.png](../tc-screenshots/full-tc-20260923/TC-111-order.png)<br>[TC-111-hide.png](../tc-screenshots/full-tc-20260923/TC-111-hide.png) |
| TC-112 | 신규 카메라를 PIP 위치 또는 전체 화면으로 배치 | 부분 확인 | PIP 및 전체 화면 배치 프리뷰 확인. 카메라 녹화는 TC-127 종료 정지/0바이트 문제로 결과 검증 실패. | [TC-112-camera-added.png](../tc-screenshots/full-tc-20260923/TC-112-camera-added.png)<br>[TC-112-fullscreen-layout.png](../tc-screenshots/full-tc-20260923/TC-112-fullscreen-layout.png) |
| TC-113 | 소스 추가 중 취소 또는 기존 소스 제거 | 부분 확인 | 시험 중 추가한 카메라만 제거되고 기존 이미지 소스 유지 확인. 소스 추가 취소 분기는 미확인. | [TC-113-removed.png](../tc-screenshots/full-tc-20260923/TC-113-removed.png) |
| TC-114 | 카메라 2대를 연결하고 장치 전환 | 차단 | 실제 카메라 1대(C920) 확인. 서로 다른 2대 전환에 필요한 두 번째 장치 없음. |  |
| TC-115 | 카메라 해상도/FPS 변경 후 적용 | 실패 | UI 요약은 1280x720로 갱신됐으나 실제 카메라 로그는 640x480 그대로이며 변경 적용 로그 없음. 이후 녹화 파일 0바이트/앱 종료 정지 동반. 원인 분리 필요. | [TC-115-applied.png](../tc-screenshots/full-tc-20260923/TC-115-applied.png)<br>[TC-127-shutdown-hang.png](../tc-screenshots/full-tc-20260923/TC-127-shutdown-hang.png) |
| TC-116 | 선택한 카메라를 사용 중 분리 후 재연결 | 차단 | 물리 카메라 분리·재연결을 수행할 사용자 조작 필요. |  |
| TC-117 | 카메라 장치 세부 설정 열기 | 부분 확인 | C920 드라이버 속성 창 진입·취소 복귀 확인. 실제 드라이버 값 변경/적용은 미수행. | [TC-117-driver.png](../tc-screenshots/full-tc-20260923/TC-117-driver.png)<br>[TC-117-driver-cancel.png](../tc-screenshots/full-tc-20260923/TC-117-driver-cancel.png) |
| TC-118 | 마이크와 PC 소리 장치 선택 후 소리 입력 | 부분 확인 | PC 소리/마이크 장치별 레벨·음소거·장치 선택 UI 확인. 마이크 실음성 입력과 분리된 두 신호 주입은 미실행. | [TC-118-audio.png](../tc-screenshots/full-tc-20260923/TC-118-audio.png) |
| TC-119 | 입력·모니터링·녹음 볼륨 변경 후 짧게 녹화 | 차단 | 실제 모니터링 음량의 청감 확인 및 별도 마이크 신호가 필요. 입력·모니터링·출력 컨트롤 표시만 확인했으며 음량 검증으로 간주하지 않음. | [TC-118-audio.png](../tc-screenshots/full-tc-20260923/TC-118-audio.png) |
| TC-120 | PC 소리 캡처를 켜고/끄고 각각 녹화 | 차단 | PC 소리 포함/제외 파일의 실소리 비교 미수행. 현재 녹화는 PC/마이크 음소거 상태. 음성 검증 가능한 입력 환경에서 재시험 필요. | [TC-118-audio.png](../tc-screenshots/full-tc-20260923/TC-118-audio.png) |
| TC-121 | 자동 음량 조절을 켜고/끄고 설정 저장 | 부분 확인 | PC 소리 자동 조절 ON 시 해당 소스에 EduTool 자동 음량 압축 필터가 생성됨. OFF로 복원. 앱 재시작/마이크 독립 적용은 미확인. | [TC-121-auto-on.png](../tc-screenshots/full-tc-20260923/TC-121-auto-on.png)<br>[TC-121-filter-added.png](../tc-screenshots/full-tc-20260923/TC-121-filter-added.png)<br>[TC-121-auto-off.png](../tc-screenshots/full-tc-20260923/TC-121-auto-off.png) |
| TC-122 | 노이즈 게이트 등 오디오 필터 추가·순서 변경·비활성화 | 통과 | 데스크탑 오디오에 노이즈 게이트 추가→압축 필터 앞으로 이동→비활성화→닫고 재진입 후 순서/비활성 상태 유지. 시험 필터 제거 완료. | [TC-122-added.png](../tc-screenshots/full-tc-20260923/TC-122-added.png)<br>[TC-122-order.png](../tc-screenshots/full-tc-20260923/TC-122-order.png)<br>[TC-122-disabled.png](../tc-screenshots/full-tc-20260923/TC-122-disabled.png)<br>[TC-122-reopened.png](../tc-screenshots/full-tc-20260923/TC-122-reopened.png) |
| TC-123 | 지연 녹화 없이 시작 | 부분 확인 | 지연 0에서 즉시 REC 시간은 증가했으나 C920 설정 변경 후 생성 파일이 0바이트로 남음. 이전 지연 녹화는 정상 파일 생성. | [TC-123-immediate-camera.png](../tc-screenshots/full-tc-20260923/TC-123-immediate-camera.png)<br>[TC-127-shutdown-hang.png](../tc-screenshots/full-tc-20260923/TC-127-shutdown-hang.png) |
| TC-124 | 지연 녹화 값을 설정하고 시작 | 통과 | 10초 카운트다운 시작 직후 새 파일 없음. 지연 경과 후 REC 시간 증가와 새 파일 확인. | [TC-124-countdown.png](../tc-screenshots/full-tc-20260923/TC-124-countdown.png)<br>[TC-124-before-start.json](../tc-screenshots/full-tc-20260923/TC-124-before-start.json)<br>[TC-124-recording.png](../tc-screenshots/full-tc-20260923/TC-124-recording.png) |
| TC-125 | 녹화 중 시작 버튼 반복 클릭 | 미실행 | 실행 대기 |  |
| TC-126 | 녹화 종료 | 부분 확인 | 녹화 종료 후 REC 해제·완료 파일 경로 알림·파일 생성 확인. 이번 생성 파일의 실제 앱 재생은 후속 확인 대상. | [TC-126-complete.png](../tc-screenshots/full-tc-20260923/TC-126-complete.png)<br>[record-probe.json](../tc-screenshots/full-tc-20260923/record-probe.json) |
| TC-127 | 녹화 중 앱 종료 요청 | 실패 | 退出 확인 후 Shutting down 단계에서 응답 정지. REC 26초 화면이 수 분간 남고 새 MP4가 0바이트이며 File output complete 로그 없음. 취소 분기는 정상이나 종료 시 파일 안전 마무리 실패. | [TC-127-cancel.png](../tc-screenshots/full-tc-20260923/TC-127-cancel.png)<br>[TC-127-active-warning-2.png](../tc-screenshots/full-tc-20260923/TC-127-active-warning-2.png)<br>[TC-127-shutdown-hang.png](../tc-screenshots/full-tc-20260923/TC-127-shutdown-hang.png) |
| TC-128 | 저장 폴더 아이콘 또는 현재 경로 클릭 | 통과 | 현재 test-recordings 경로로 Windows 탐색기 열림. 실제 생성 녹화 파일 목록 확인. | [TC-128-explorer-1.png](../tc-screenshots/full-tc-20260923/TC-128-explorer-1.png) |
| TC-129 | 저장 위치 `변경` 클릭 | 통과 | 변경 클릭 시 폴더 선택 창만 열림. 취소 후 경로 그대로 유지. | [TC-129-folder-dialog.png](../tc-screenshots/full-tc-20260923/TC-129-folder-dialog.png)<br>[TC-129-cancel-path.png](../tc-screenshots/full-tc-20260923/TC-129-cancel-path.png) |
| TC-130 | 저장 위치·여유 공간 표시 확인 | 부분 확인 | 현재 C 드라이브 남은 공간 78.4GB가 실제 84224569344바이트와 일치. 드라이브 교체 갱신 분기는 미확인. | [TC-129-cancel-path.png](../tc-screenshots/full-tc-20260923/TC-129-cancel-path.png) |
| TC-201 | 정상 이메일/비밀번호로 로그인 | 차단 | 시험용 홈페이지/API·계정 미제공. Computer Use 스킬에서 인증 대화상자 자동 조작을 금지하므로 사용자 로그인 후 재시험 필요. |  |
| TC-202 | 잘못된 비밀번호 또는 필수값 누락으로 로그인 | 차단 | 시험용 홈페이지/API·계정 미제공. Computer Use 스킬에서 인증 대화상자 자동 조작을 금지하므로 사용자 로그인 후 재시험 필요. |  |
| TC-203 | 비밀번호 표시/숨기기 전환 | 차단 | 시험용 홈페이지/API·계정 미제공. Computer Use 스킬에서 인증 대화상자 자동 조작을 금지하므로 사용자 로그인 후 재시험 필요. |  |
| TC-204 | 로그인 상태 유지 선택/해제 후 앱 재실행 | 차단 | 시험용 홈페이지/API·계정 미제공. Computer Use 스킬에서 인증 대화상자 자동 조작을 금지하므로 사용자 로그인 후 재시험 필요. |  |
| TC-205 | Google/Microsoft 로그인 시작 후 취소 또는 실패 | 차단 | 시험용 홈페이지/API·계정 미제공. Computer Use 스킬에서 인증 대화상자 자동 조작을 금지하므로 사용자 로그인 후 재시험 필요. |  |
| TC-206 | 로그인 후 계정 정보 열기 | 차단 | 시험용 홈페이지/API·계정 미제공. Computer Use 스킬에서 인증 대화상자 자동 조작을 금지하므로 사용자 로그인 후 재시험 필요. |  |
| TC-207 | 계정 메뉴에서 로그아웃 | 차단 | 시험용 홈페이지/API·계정 미제공. Computer Use 스킬에서 인증 대화상자 자동 조작을 금지하므로 사용자 로그인 후 재시험 필요. |  |
| TC-208 | 연결 홈페이지 변경 | 차단 | 시험용 홈페이지/API·계정 미제공. Computer Use 스킬에서 인증 대화상자 자동 조작을 금지하므로 사용자 로그인 후 재시험 필요. |  |
| TC-209 | 사용 중 세션 만료 | 차단 | 시험용 홈페이지/API·계정 미제공. Computer Use 스킬에서 인증 대화상자 자동 조작을 금지하므로 사용자 로그인 후 재시험 필요. |  |
| TC-301 | 연결된 홈페이지의 강좌·폴더 목록 열기 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-302 | 강좌·폴더 선택 후 다시 열기 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-303 | 새 폴더 생성 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-304 | 제목 규칙 적용 및 동일 이름의 파일 업로드 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-305 | 자동 업로드 OFF로 녹화 종료 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-306 | 자동 업로드 ON으로 녹화 종료 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-307 | 업로드 설정 저장 후 앱 재시작 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-308 | 유효한 영상 수동 업로드 | 차단 | 완료 영상 전달 후 업로드 시 로그인/대상 폴더 필요 안내. 사전조건 검사는 확인했으나 실제 업로드는 시험 서버·계정 부재로 차단. | [TC-529-handoff.png](../tc-screenshots/full-tc-20260923/TC-529-handoff.png)<br>[TC-308-no-account.png](../tc-screenshots/full-tc-20260923/TC-308-no-account.png) |
| TC-309 | 업로드 중 네트워크 끊기 후 복구 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-310 | 서버 인증 실패 또는 권한 없는 폴더 선택 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-311 | 업로드 중 앱 종료·재시작 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-312 | 원본 자동 삭제 옵션 선택 후 업로드 실패 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-313 | 원본 자동 삭제 옵션 선택 후 서버 업로드 검증 성공 | 차단 | 시험용 홈페이지/API·로그인·업로드 대상 미제공. 실제 서버 결과를 확인할 수 없음. |  |
| TC-401 | 라이브 화면 진입 | 부분 확인 | 라이브 진입 시 실제 카메라 프리뷰와 REC 시간 유지. 실제 LIVE 시간 및 마이크 신호는 미확인. | [TC-404-live-shared-camera.png](../tc-screenshots/full-tc-20260923/TC-404-live-shared-camera.png) |
| TC-402 | YouTube 계정 연결/해제 | 차단 | 시험용 방송 연결/계정·비공개 방송 대상 미제공. 실제 송출·서버 결과 미확인. |  |
| TC-403 | 연결 설정 진입 후 변경·취소 | 차단 | 시험용 방송 연결/계정·비공개 방송 대상 미제공. 실제 송출·서버 결과 미확인. |  |
| TC-404 | 라이브 화면에서 소스 관리/장치 세부 설정 진입 | 통과 | 녹화에서 추가한 C920가 라이브 프리뷰와 소스 관리에 동일하게 나타남. 라이브에서 전체 화면 배치·순서·숨김 변경 즉시 반영. | [TC-404-live-shared-camera.png](../tc-screenshots/full-tc-20260923/TC-404-live-shared-camera.png)<br>[TC-112-fullscreen-layout.png](../tc-screenshots/full-tc-20260923/TC-112-fullscreen-layout.png)<br>[TC-111-hide.png](../tc-screenshots/full-tc-20260923/TC-111-hide.png) |
| TC-405 | 정상 연결 상태에서 라이브 시작 | 차단 | 시험용 방송 연결/계정·비공개 방송 대상 미제공. 실제 송출·서버 결과 미확인. |  |
| TC-406 | YouTube 미연결/유효하지 않은 방송 설정으로 시작 | 통과 | URL/스트림 키 미설정 시 방송 시작 차단·방송 설정 누락 원인과 설정 열기/취소 표시. | [TC-406-unconfigured.png](../tc-screenshots/full-tc-20260923/TC-406-unconfigured.png) |
| TC-407 | 라이브 중 녹화 시작·종료 | 차단 | 시험용 방송 연결/계정·비공개 방송 대상 미제공. 실제 송출·서버 결과 미확인. |  |
| TC-408 | 라이브 종료 | 차단 | 시험용 방송 연결/계정·비공개 방송 대상 미제공. 실제 송출·서버 결과 미확인. |  |
| TC-409 | 라이브 중 네트워크 또는 장치 장애 | 차단 | 시험용 방송 연결/계정·비공개 방송 대상 미제공. 실제 송출·서버 결과 미확인. |  |
| TC-501 | 정상 MP4를 파일 선택으로 열기 | 미실행 | 실행 대기 |  |
| TC-502 | 정상 영상을 창에 끌어놓기 | 미실행 | 실행 대기 |  |
| TC-503 | 최근 파일에서 영상 열기 | 미실행 | 실행 대기 |  |
| TC-504 | 손상된 파일 또는 지원하지 않는 코덱 열기 | 미실행 | 실행 대기 |  |
| TC-505 | 두 번째 영상을 추가하거나 새로 불러오기 | 미실행 | 실행 대기 |  |
| TC-506 | 재생·일시정지·임의 시점 탐색 | 미실행 | 실행 대기 |  |
| TC-507 | 볼륨 변경·전체화면 진입/종료 | 미실행 | 실행 대기 |  |
| TC-508 | 타임라인 확대/축소 | 미실행 | 실행 대기 |  |
| TC-509 | `전체 보기` 선택 | 미실행 | 실행 대기 |  |
| TC-510 | 긴 영상에서 타임라인 스크롤·탐색 | 미실행 | 실행 대기 |  |
| TC-511 | 마우스 드래그로 구간 선택 | 미실행 | 실행 대기 |  |
| TC-512 | 시작/끝 시간을 직접 입력 | 부분 확인 | 시작보다 작은 종료값 입력 후 선택 재생 시 오류 표시 확인. 영상 길이 초과 경계는 미확인. | [TC-512-reversed.png](../tc-screenshots/full-tc-20260923/TC-512-reversed.png)<br>[TC-512-error.png](../tc-screenshots/full-tc-20260923/TC-512-error.png) |
| TC-513 | 선택 구간 재생 후 선택 해제 | 실패 | 5.025~9.982초 선택 재생 후 검은 프리뷰·시간 5.025초 정지. 선택 끝까지 재생되지 않음. | [TC-513-play.png](../tc-screenshots/full-tc-20260923/TC-513-play.png)<br>[TC-513-end.png](../tc-screenshots/full-tc-20260923/TC-513-end.png) |
| TC-514 | 선택 구간 삭제 | 부분 확인 | 1~2초 삭제 후 73.4→72.4초, 3→4개 클립과 경계 갱신. 결과 내보내기 72.429667초/정상 디코드. 앱 프리뷰의 경계 재생은 미확인. | [TC-514-cut.png](../tc-screenshots/full-tc-20260923/TC-514-cut.png)<br>[export-probe.json](../tc-screenshots/full-tc-20260923/export-probe.json) |
| TC-515 | 삭제 후 실행 취소/다시 실행 | 실패 | 실행 취소 시 73.4초/3클립 복원되지만 프리뷰가 검게 남음. 다시 실행 시 72.4초/4클립 복원. 영상 결과 복원 기대 미충족. | [TC-515-undo.png](../tc-screenshots/full-tc-20260923/TC-515-undo.png)<br>[TC-515-redo.png](../tc-screenshots/full-tc-20260923/TC-515-redo.png) |
| TC-516 | 영상 중간의 재생 위치에서 나누기 | 미실행 | 실행 대기 |  |
| TC-517 | 여러 번 나누고 조각 이름 수정 | 미실행 | 실행 대기 |  |
| TC-518 | 조각 하나를 저장 | 미실행 | 실행 대기 |  |
| TC-519 | 0초 또는 영상 끝에서 나누기 | 부분 확인 | 0초 분할은 오류 안내와 함께 제한됨. 영상 끝 지점 분기는 미확인. | [TC-519-zero.png](../tc-screenshots/full-tc-20260923/TC-519-zero.png) |
| TC-520 | 복수 영상을 추가해 앞/현재 위치/뒤에 각각 삽입 | 미실행 | 실행 대기 |  |
| TC-521 | 합친 영상의 경계 전후 재생 | 미실행 | 실행 대기 |  |
| TC-522 | 서로 다른 해상도/FPS/코덱의 영상 합치기 | 미실행 | 실행 대기 |  |
| TC-523 | 합치기 실행 취소/다시 실행 | 미실행 | 실행 대기 |  |
| TC-524 | 편집 작업 저장 후 앱 재시작·다시 열기 | 부분 확인 | 이전 저장 작업을 앱 재시작 후 열어 3클립/73.4초·분할·합치기·선택 범위 복원 확인. 사용자 이름 변경 및 컷 저장 상태는 미확인. | [TC-524-restored.png](../tc-screenshots/full-tc-20260923/TC-524-restored.png) |
| TC-525 | 원본 파일 이동/삭제 후 저장 작업 열기 | 미실행 | 실행 대기 |  |
| TC-526 | 결과 영상 내보내기 | 부분 확인 | 4클립/72.4초 작업 내보내기 성공. H264/AAC 1280x720 30fps 72.429667초, 전체 디코드 오류 없음. 실제 앱 재생·소리 비교는 미완료. | [TC-526-export-complete.png](../tc-screenshots/full-tc-20260923/TC-526-export-complete.png)<br>[export-probe.json](../tc-screenshots/full-tc-20260923/export-probe.json) |
| TC-527 | 내보내기 경로 쓰기 불가/파일명 충돌 | 미실행 | 실행 대기 |  |
| TC-528 | 내보내기 중 취소/오류 | 미실행 | 실행 대기 |  |
| TC-529 | 내보낸 영상을 바로 업로드 | 차단 | 완료 파일 cut-export.mp4와 제목이 업로드 화면에 전달됨. 서버 실제 결과 일치는 환경 부재로 확인 불가. | [TC-529-handoff.png](../tc-screenshots/full-tc-20260923/TC-529-handoff.png)<br>[TC-308-no-account.png](../tc-screenshots/full-tc-20260923/TC-308-no-account.png) |

# 전체 TC 재점검 — 참고 이미지 1:1 비교 (2026-09-23)

[24장 원본과 실제 화면 병렬 보기](../tc-screenshots/full-tc-20260923/comparison.html) · [전체 97개 TC 결과](edutool-tc-results-20260923.md)

기준 브랜치 `EduTool_Studio`, 검증 코드 `bce57e9ab`(실행 코드 8dd635e65와 동일). 실제 Windows UI를 Computer Use로 조작했다. 참고 이미지의 지시문은 비교 자료로만 읽었으며 메인 공통 설정 내용은 제외했다.

## 집계와 한계

통과 **25/97 (25.8%)** = 이번 확인21 + 기존 통과4. 실패9, 부분 확인29, 차단34. 전체 항목을 결과표에 등록했으나 전체 시나리오 실행/통과가 완료된 상태는 아니다. 일부 절차·조건이 남은 항목은 부분 확인으로 유지했다.

이미지별 판정은 레이아웃 비교이며 TC 통과 수와 다르다. 카메라 C920 한 대와 합성 시험 영상을 사용했다. 인증·서버 업로드·실제 방송·청감·물리 분리 및 특수 디스크 환경은 별도 조건이 필요하다.

| 참고 | 요청 내용 | 관련 TC | 판정 | 실제 관찰·차이·남은 조건 | 최신 증거 |
|---|---|---|---|---|---|
| [01 녹화 폴더](../tc-screenshots/full-tc-20260923/comparison.html#ref-01) | 현재 저장 경로·여유 공간·폴더 열기·경로 변경 | TC-128~130 | 부분 확인 | 탐색기 실제 파일 목록과 경로 선택 취소를 확인했다. 여유 공간 표시는 실제 C: 여유 공간과 대조했다. 경로 바는 참고의 하단 대신 우측 패널이며 볼륨 변경은 미검증. | [TC-128-explorer-1.png](../tc-screenshots/full-tc-20260923/TC-128-explorer-1.png)<br>[TC-129-cancel-path.png](../tc-screenshots/full-tc-20260923/TC-129-cancel-path.png) |
| [02 메인 공통 고급설정](../tc-screenshots/full-tc-20260923/comparison.html#ref-02) | 메인 공통 설정 내용 | 제외 | 제외 | 사용자 요청에 따라 메인 공통 설정 콘텐츠는 비교·구현 범위에서 제외한다. |  |
| [03 녹화 설정](../tc-screenshots/full-tc-20260923/comparison.html#ref-03) | 경로·품질·형식·지연·타이머 팝업·초기화·취소/저장 | TC-105~109 | 부분 확인 | 기능별 녹화 설정과 기본값 초기화·취소를 실제 조작했다. 권한 거부/저용량 볼륨은 미시험. 지연 녹화 10초는 별도 TC-124 통과. 참고의 카드형 팝업과 배치는 다르다. | [TC-107-record-settings.png](../tc-screenshots/full-tc-20260923/TC-107-record-settings.png)<br>[TC-107-reset.png](../tc-screenshots/full-tc-20260923/TC-107-reset.png) |
| [04 소스 관리](../tc-screenshots/full-tc-20260923/comparison.html#ref-04) | 소스 종류·목록·미리보기·PIP 위치 선택 | TC-110~113 | 부분 확인 | 실제 C920 추가, PIP/전체 배치, 숨김/순서 변경, 시험 소스 제거를 조작했다. 다른 소스 유형 전체 및 녹화 프레임 대조는 미완료. 소스 관리 창이 새 속성 창을 가리는 현상도 관찰했다. | [TC-110-source-manager.png](../tc-screenshots/full-tc-20260923/TC-110-source-manager.png)<br>[TC-112-camera-added.png](../tc-screenshots/full-tc-20260923/TC-112-camera-added.png)<br>[TC-111-order.png](../tc-screenshots/full-tc-20260923/TC-111-order.png) |
| [05 업로드 설정](../tc-screenshots/full-tc-20260923/comparison.html#ref-05) | 자동 업로드·대상 폴더·제목 규칙·알림·원본 보존 | TC-301~313 | 부분 확인 | 완료 알림 OFF 변경 후 재시작 유지, ON 원복을 확인했다. 실제 서버/계정/업로드는 차단. 참고의 별도 설정 팝업 대신 로그인과 통합된 화면이다. | [TC-307-restarted.png](../tc-screenshots/full-tc-20260923/TC-307-restarted.png)<br>[TC-308-no-account.png](../tc-screenshots/full-tc-20260923/TC-308-no-account.png) |
| [06 오디오 필터](../tc-screenshots/full-tc-20260923/comparison.html#ref-06) | 마이크 필터 목록·노이즈 게이트 세부 값 | TC-122 | 부분 확인 | PC 소리 필터에 노이즈 게이트를 추가·순서 변경·비활성화하고 재진입 유지 확인. 시험 필터는 제거했다. 참고의 마이크 필터와 입력 대상이 다르며 OBS 기본 스킨을 사용한다. | [TC-122-added.png](../tc-screenshots/full-tc-20260923/TC-122-added.png)<br>[TC-122-reopened.png](../tc-screenshots/full-tc-20260923/TC-122-reopened.png) |
| [07 오디오 설정](../tc-screenshots/full-tc-20260923/comparison.html#ref-07) | 마이크/PC 소리 카드·입력/모니터링/녹음 음량·미터 | TC-118~121 | 부분 확인 | PC 자동 음량 ON에서 압축기 추가를 확인하고 OFF 원복했다. 입력별 설정은 표시되나 실제 마이크 신호/독립 음량/모니터링 청감은 미확인. | [TC-118-audio.png](../tc-screenshots/full-tc-20260923/TC-118-audio.png)<br>[TC-121-auto-on.png](../tc-screenshots/full-tc-20260923/TC-121-auto-on.png) |
| [08 카메라 설정](../tc-screenshots/full-tc-20260923/comparison.html#ref-08) | 카메라 영상·장치·해상도/FPS·초기화·적용 | TC-114~116 | 부분 확인 | C920 실제 영상과 해상도 선택·요약 갱신을 확인했다. 재시작 후720p 초기화 확인. 즉시 반영과 녹화 결과 대조, 2대 전환, 물리 분리/재연결은 미완료. | [TC-114-camera-settings.png](../tc-screenshots/full-tc-20260923/TC-114-camera-settings.png)<br>[TC-115-applied.png](../tc-screenshots/full-tc-20260923/TC-115-applied.png) |
| [09 카메라 장치 세부설정](../tc-screenshots/full-tc-20260923/comparison.html#ref-09) | 카메라 드라이버 속성 창과 적용/취소 | TC-117 | 부분 확인 | C920 드라이버 속성 창을 실제 열고 취소했다. 참고와 장치/드라이버가 달라 제어 항목이 다르며 실제 드라이버 값 변경은 미검증. | [TC-117-driver-1.png](../tc-screenshots/full-tc-20260923/TC-117-driver-1.png) |
| [10 녹화 시작](../tc-screenshots/full-tc-20260923/comparison.html#ref-10) | REC·경과 시간·중지 버튼·장치 상태 | TC-123~125 | 부분 확인 | 즉시 시작과10초 카운트다운 후 REC·파일 생성 확인. 중복 시작 버튼은 중지로 바뀜. 연타/단축키 중복 요청은 미검증. 참고의 하단 버튼과 실제 배치 차이가 있다. | [TC-123-immediate-camera.png](../tc-screenshots/full-tc-20260923/TC-123-immediate-camera.png)<br>[TC-124-countdown.png](../tc-screenshots/full-tc-20260923/TC-124-countdown.png) |
| [11 녹화 종료](../tc-screenshots/full-tc-20260923/comparison.html#ref-11) | 자동 업로드 OFF 완료 알림 / ON 진행률·완료 | TC-126, TC-305~306 | 차이/결함 확인 | 정상 중지 및 완료 경로 표시는 확인. 녹화 중 종료 확인을 승인하면 앱이 멈추고 녹화가 계속되는 결함을 재현했다. 강제 종료 후 파일은 남았으므로 손상으로 단정하지 않는다. 자동 업로드 ON은 미검증. | [TC-126-complete.png](../tc-screenshots/full-tc-20260923/TC-126-complete.png)<br>[TC-127-shutdown-hang.png](../tc-screenshots/full-tc-20260923/TC-127-shutdown-hang.png) |
| [12 우측 패널 접기](../tc-screenshots/full-tc-20260923/comparison.html#ref-12) | 패널 축소 시 아이콘 레일·미리보기 확대 | TC-103 | 부분 확인 | 패널 접기/펴기 정상. 참고의 좁은 아이콘 레일 대신 패널 전체가 숨겨진다. | [TC-103-panel-open.png](../tc-screenshots/full-tc-20260923/TC-103-panel-open.png) |
| [13 녹화 메인](../tc-screenshots/full-tc-20260923/comparison.html#ref-13) | 상단 모드·큰 프리뷰·장치 카드·하단 실행/폴더 바 | TC-001~003, TC-101 | 차이/결함 확인 | 재시작 시 OBS 도크가 편집 영역에 남고 모드 전환 후 제어 도크가 사라지는 실패. 수동 도크 복구가 필요하다. 참고와 레이아웃 차이가 크다. | [TC-001-record.png](../tc-screenshots/full-tc-20260923/TC-001-record.png)<br>[startup.png](../tc-screenshots/full-tc-20260923/startup.png) |
| [14 라이브 메인](../tc-screenshots/full-tc-20260923/comparison.html#ref-14) | YouTube 연결 카드·LIVE/REC 독립 시간·방송/녹화 버튼 | TC-401~409 | 부분 확인 | 실제 카메라가 녹화/라이브에서 공유되는 것과 미설정 방송 시작의 원인 안내 확인. 실제 송출은 비공개 시험 대상·계정/키 부재로 차단. | [TC-404-live-shared-camera.png](../tc-screenshots/full-tc-20260923/TC-404-live-shared-camera.png)<br>[TC-406-unconfigured.png](../tc-screenshots/full-tc-20260923/TC-406-unconfigured.png) |
| [15 라이브 기능별 설정](../tc-screenshots/full-tc-20260923/comparison.html#ref-15) | 소스 관리 및 기능별 방송 설정; 공통 고급설정 부분은 제외 | TC-403~404 | 부분 확인 | 라이브에서 소스 관리와 표시/배치 공유 확인. 방송 적용 값의 실제 연결 사용은 서버 미설정으로 미검증. 공통 고급설정은 제외한다. | [TC-111-manager.png](../tc-screenshots/full-tc-20260923/TC-111-manager.png) |
| [16 로그인 후 계정 정보](../tc-screenshots/full-tc-20260923/comparison.html#ref-16) | 계정 메뉴·이름/이메일·홈페이지·계정 유형·가입/접속일 | TC-206~209 | 차단 | 인증된 계정이 없어 로그인 후 상태를 재현하지 못했다. 오른쪽 증거는 로그인 전 차단 조건이며 완성 화면의 대체물이 아니다. | [TC-001-upload.png](../tc-screenshots/full-tc-20260923/TC-001-upload.png) |
| [17 상단 로그인](../tc-screenshots/full-tc-20260923/comparison.html#ref-17) | 이메일·비밀번호·유지·Google/Microsoft·가입/비밀번호 찾기 | TC-201~205 | 차단 | 연결/입력 화면을 관찰했으나 인증 조작은 하지 않았다. Computer Use 스킬의 인증 자동 조작 금지와 시험 계정/서버 부재로 사용자 로그인 필요. | [TC-001-upload.png](../tc-screenshots/full-tc-20260923/TC-001-upload.png) |
| [18 편집 구간 선택](../tc-screenshots/full-tc-20260923/comparison.html#ref-18) | 시작/끝/길이·선택 강조·구간 재생·삭제 | TC-511~515 | 차이/결함 확인 | 선택 강조 및 시작/끝 입력, 역전 오류, 삭제·undo/redo 조작. 선택 재생과 undo 후 검은 미리보기 실패. 선택 길이의 별도 표시가 없다. | [TC-511-selection.png](../tc-screenshots/full-tc-20260923/TC-511-selection.png)<br>[TC-513-end.png](../tc-screenshots/full-tc-20260923/TC-513-end.png)<br>[TC-515-undo.png](../tc-screenshots/full-tc-20260923/TC-515-undo.png) |
| [19 편집 영상 나누기](../tc-screenshots/full-tc-20260923/comparison.html#ref-19) | 조각별 이름·시작/끝/길이·저장 | TC-516~519 | 부분 확인 | 3조각 분할·첫 조각 이름 변경·경계 유지 확인. 중간 조각만 저장해 약2초 H264/AAC 출력 확인. 0초 분할 거부 확인, 끝 경계 및 실제 출력 음향은 미완료. | [TC-517-multiple.png](../tc-screenshots/full-tc-20260923/TC-517-multiple.png)<br>[TC-518-export.png](../tc-screenshots/full-tc-20260923/TC-518-export.png) |
| [20 편집 영상 합치기](../tc-screenshots/full-tc-20260923/comparison.html#ref-20) | 파일 선택·앞/현재/뒤 삽입·순서·길이 | TC-520~523 | 차이/결함 확인 | 앞/뒤 추가를 확인했고 현재 위치 삽입은 미검증. 24fps MP4+25fps AVI는1280×720 30fps로 내보내기 성공. 편집 중 조각 경계에서 검은 화면 정지, undo 위치0초 오류 발생. | [TC-520-front.png](../tc-screenshots/full-tc-20260923/TC-520-front.png)<br>[TC-521-stalled.png](../tc-screenshots/full-tc-20260923/TC-521-stalled.png)<br>[TC-522-complete.png](../tc-screenshots/full-tc-20260923/TC-522-complete.png) |
| [21 편집 확대·축소·전체 보기](../tc-screenshots/full-tc-20260923/comparison.html#ref-21) | 확대/축소·전체 맞춤·선택 및 위치 유지 | TC-508~510 | 부분 확인 | 8초 영상 선택/위치 유지한 확대·전체 보기와10분 영상5배 확대·스크롤·307초 탐색을 확인했다. 영상 내 시간과1fps 범위 내 일치. 고해상도 부하 시험은 별도. | [TC-508-zoom.png](../tc-screenshots/full-tc-20260923/TC-508-zoom.png)<br>[TC-510-scroll.png](../tc-screenshots/full-tc-20260923/TC-510-scroll.png)<br>[TC-510-seek.png](../tc-screenshots/full-tc-20260923/TC-510-seek.png) |
| [22 편집 빈 화면](../tc-screenshots/full-tc-20260923/comparison.html#ref-22) | 가져오기 안내·큰 시작 버튼·최근 영상 카드·초기 비활성 도구 | TC-501~505 | 부분 확인 | 빈 편집 화면과 일반 가져오기 경로 확인. 참고의 안내/최근 썸네일 카드가 없고 공통 버튼이 나열된다. | [TC-501-empty.png](../tc-screenshots/full-tc-20260923/TC-501-empty.png) |
| [23 영상 불러오기 창](../tc-screenshots/full-tc-20260923/comparison.html#ref-23) | 영상 파일 필터·썸네일·파일 선택 | TC-501~505 | 차이/결함 확인 | 기본 파일 대화상자는 실행 폴더/All Files로 열림. 손상 파일 추가 시 기존 작업 유지. 누락 최근 파일은 일반 오류만 표시하며 위치 찾기/목록 정리 안내가 없다. 창간 드롭은 도구가 좌표를 거부해 차단. | [TC-505-new-prompt.png](../tc-screenshots/full-tc-20260923/TC-505-new-prompt.png)<br>[TC-504-invalid-preserved.png](../tc-screenshots/full-tc-20260923/TC-504-invalid-preserved.png)<br>[TC-503-error.png](../tc-screenshots/full-tc-20260923/TC-503-error.png) |
| [24 영상 불러오기 완료](../tc-screenshots/full-tc-20260923/comparison.html#ref-24) | 미리보기·메타데이터·타임라인 썸네일·재생 | TC-501, TC-506~507 | 차이/결함 확인 | 24fps 시험 영상의 미리보기 재생·시간 진행 확인. 메타데이터는 가로로 잘림. 전체화면 후 떠 있는 창으로 남으며 복귀/닫기 후 편집 미리보기 영역이 사라지는 실패. 실제 음향 동기 확인은 미완료. | [TC-506-play.png](../tc-screenshots/full-tc-20260923/TC-506-play.png)<br>[TC-507-focus-escape.png](../tc-screenshots/full-tc-20260923/TC-507-focus-escape.png) |

## 재현한 실패

| TC | 결과 |
|---|---|
| TC-001 | 재시작 후 편집 화면에 OBS 도크가 겹쳐 표시됨. 모드 전환 후 제어 도크가 사라져 독 메뉴에서 수동 복구해야 녹화 시작 가능. |
| TC-003 | 편집 화면의 주요 버튼이 가로로 잘림. 재시작 시 기존 OBS 도크가 편집 영역을 차지함. |
| TC-127 | 종료 확인 후 Shutting down 단계에서 응답 정지(REC 26초 화면 고정). 수 분 후 시험 프로세스 강제 종료 필요. 실행 중 파일 크기는 0바이트로 관찰됐으나 종료 후 137,637,928바이트/183.367초 H264·AAC 파일이 남음. 즉 파일 없음/손상으로 단정하지 않으며 정상 종료·녹화 정지 실패로 판정. |
| TC-503 | 이동한 원본을 최근 목록에서 선택하면 영상 또는 ffprobe를 찾을 수 없다는 일반 오류만 표시. 누락 항목 정리/위치 찾기 안내가 없고 목록에 그대로 남음. 기존 편집은 유지. |
| TC-507 | 전체화면 진입 후 Escape 복귀 시 영상 위젯이 편집 영역으로 돌아오지 않고 별도 obs64 창으로 남음. 해당 창 닫으면 편집 미리보기 자체가 사라짐. |
| TC-513 | 5.025~9.982초 선택 재생 후 검은 프리뷰·시간 5.025초 정지. 선택 끝까지 재생되지 않음. |
| TC-515 | 실행 취소 시 73.4초/3클립 복원되지만 프리뷰가 검게 남음. 다시 실행 시 72.4초/4클립 복원. 영상 결과 복원 기대 미충족. |
| TC-521 | 3초 AVI 뒤에 분할 MP4를 합쳐 2.475초부터 재생. 6.979초(분할 조각 경계)에서 검은 미리보기와 시간이 멈추고 재관찰에도 동일. |
| TC-523 | 앞에 3초 AVI 삽입 전 위치5.993초. 실행 취소로 3클립/8초/순서는 복원되나 위치가0초로 바뀜. 다시 실행은4클립/11초 복원하나 정지 미리보기가 검게 표시됨. |

## 증거 관리

- 스크린샷 원본과 UI 관찰 JSON: `tc-screenshots/full-tc-20260923/`. 기존 `.gitignore` 규칙 적용. 원본 캡처를 합성하거나 변경하지 않았다.
- 원본 ZIP 24장: 기존 `tc-screenshots/comparison-20260922/reference/`를 그대로 연결했다.
- 파일별 SHA-256은 최신 증거 폴더의 `manifest.json`에 기록했다.
- HTML은 이미지 링크와 24개 대응 구조를 정적으로 확인했다. 브라우저 로컬 파일 접근 제한으로 렌더링·브라우저 조작 검사는 미수행이다.
- 실행 중 0바이트로 보였던 녹화는 종료 후137,637,928바이트/183.367초로 확인됐다. TC-127은 정상 종료/녹화 정지 실패이며 파일 손상으로 단정하지 않는다.
- 완료 알림은ON, 자동 업로드OFF, 원본 삭제OFF로 원복했다. 시험 카메라 소스와 노이즈 게이트를 제거했고 시험용 원본 이름을 복구했다.
- 발견한 결함을 수정한 작업이 아니라 실행 검증 및 확인자료 작성이다.

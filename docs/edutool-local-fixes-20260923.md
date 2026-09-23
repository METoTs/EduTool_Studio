# 로컬 실패 TC 수정 — 2026-09-23

## 범위

로그인·인증·서버 연결·실제 방송/업로드는 이번 수정과 재검증에서 제외한다. 기존 전체 TC 결과는 이력으로 보존한다. 메인 공통 설정 제외 조건도 유지한다.

## 구현

- 편집 화면 내부 파일 탐색기: 폴더 경로 입력, 상위/동영상 폴더 이동, 파일 형식 필터, 다중 선택, 더블클릭 가져오기, 같은 창 안에서 타임라인으로 드래그, 남은 공간 표시.
- 영상/작업 열기, 작업/영상 저장, 원본 재연결, 녹화 저장 폴더 변경/확인에 내부 탐색기 사용. 기존 영상 덮어쓰기 방지 유지.
- 편집 버튼을 여러 행으로 배치하고 최근 파일 목록의 최소 너비 및 조각 메타데이터 줄바꿈을 조정.
- 같은 원본의 분할 조각 사이 이동 시 미디어 재로딩 방지. 로딩·탐색 중 위치 이벤트와 경계 이동을 분리하여 실행 취소 위치와 선택 재생 보호.
- 전체화면 종료 시 원래 편집 영역으로 영상 위젯 복귀.
- 누락된 최근 파일에 위치 찾기/목록 정리 제공.
- 저장된 OBS 도크 복원 뒤 마지막 기능 화면 선택. 녹화/라이브 화면 진입 시 필수 제어판 복원.
- 녹화 중 앱 종료 시 중지 요청 후 파일 출력 비활성화를 기다려 종료. 저장 폴더의 실제 임시 파일 쓰기/flush 사전 확인.

## 증거 관리

스크린샷·UI 상태 JSON·빌드 로그·시험 영상은 Git 제외 경로 `tc-screenshots/fixes-20260923/` 및 `tc-screenshots/fix-build-20260923*.log`에 보관한다. 증거 중 앞 번호는 수정 도중 재현 기록을 포함하므로 최종 판정표에 연결된 증거를 기준으로 판단한다.

## 남는 환경 조건

- TC-108: 쓰기 불가 전용 시험 폴더 조건. 사전 쓰기 확인은 구현했지만 일반 폴더 성공만으로 권한 거부 시나리오를 통과 처리하지 않는다.
- TC-109: 저용량 시험 볼륨의 시작/진행 중 공간 부족 시험. 운영 디스크를 채우지 않는다.
- TC-114/116: 두 번째 카메라, 실제 케이블 분리/재연결.
- TC-119/120 및 청감 포함 항목: 실제 입력 음원/모니터링 소리 확인.

이 조건들은 코드 수정만으로 실제 환경 시험을 대체할 수 없으므로 별도 상태를 유지한다.

## 빌드 및 실제 재검증 결과

구현 커밋: `a29ce3da2` · 브랜치: `EduTool_Studio`. RelWithDebInfo `obs-studio` 최종 빌드 성공. 마지막 레이아웃 보완 빌드는 `tc-screenshots/fix-build-20260923-layout.log`에 보관. Windows SDK 접근은 승인된 권한으로 빌드했다.

정지 화면은 비동기 FFmpeg 프레임 추출로 원본 시각의 프레임을 표시한다. 연속 탐색은 60ms 지연/세대 번호로 이전 요청을 무시하며, 앱에 포함된 FFmpeg를 사용한다. 재생 시에는 Qt 영상 위젯을 사용한다.

**통과32/97(33.0%)** = 통과28 + 기존 통과4. 부분 확인32, 차단33. 기존 실패9건 중6건 통과,3건 부분 확인으로 변경. 추가로 차단 TC-502가 내부 탐색기 경로로 통과했다. 이번에 재시험하지 않은 나머지는 이전 판정을 유지하므로 전체97개를 새 빌드에서 모두 통과했다는 의미가 아니다.

| TC | 이전 → 현재 | 확인 결과 | 증거 |
|---|---|---|---|
| TC-001 | 실패 → 통과 | 재시작 편집 화면에 도크 겹침 없음. 녹화→라이브→업로드→녹화 전환 후 선택 화면과 메뉴 일치, 녹화 제어판 유지. 서버 연결 없이 대기 상태에서 확인. | [1](../tc-screenshots/fixes-20260923/45-final-start-layout.png) [2](../tc-screenshots/fixes-20260923/46-final-record.png) [3](../tc-screenshots/fixes-20260923/47-final-live-layout.png) [4](../tc-screenshots/fixes-20260923/48-final-upload-layout.png) [5](../tc-screenshots/fixes-20260923/49-record-after-switching.png) |
| TC-003 | 실패 → 부분 확인 | 일반 크기에서 버튼 여러 행 배치와 조각 메타데이터 줄바꿈 확인. 원본 해상도에 따른 미리보기 팽창을 크기 정책으로 보완. 최소 창 크기 전체 검증은 남음. | [1](../tc-screenshots/fixes-20260923/45-final-start-layout.png) [2](../tc-screenshots/fixes-20260923/53-recording-loaded.png) [3](../tc-screenshots/fixes-20260923/61-layout-after-play.png) |
| TC-127 | 실패 → 통과 | 녹화 중 종료 확인 1회 후 비동기 중지와 출력 비활성화를 기다려 정상 종료. 19.633333초/14,753,856바이트 MP4 전체 디코딩 exit 0, 종료 로그 memory leaks 0. 앞선 수정 중 재시도 기록과 구분. | [1](../tc-screenshots/fixes-20260923/18-final-exit-prompt.png) [2](../tc-screenshots/fixes-20260923/19-final-record-stop.png) [3](../tc-screenshots/fixes-20260923/record-exit-final.log) [4](../tc-screenshots/fixes-20260923/record-exit-final-probe.json) [5](../tc-screenshots/fixes-20260923/record-exit-final-decode.log) |
| TC-502 | 차단 → 통과 | 앱 내부 탐색기에서 blue-25fps.avi를 같은 창 타임라인으로 실제 드래그 후 클릭으로 드롭 확정. 맨 앞 삽입 정책에 따라 3초 AVI + 기존 8초 = 11초, 4조각으로 반영. 별도 Windows 탐색기 간 드래그의 도구 제한은 내부 경로로 대체. | [1](../tc-screenshots/fixes-20260923/22-internal-drag.png) [2](../tc-screenshots/fixes-20260923/24-drag-added-11sec.png) |
| TC-503 | 실패 → 통과 | 시험 AVI 이름을 일시 변경한 뒤 최근 목록 선택. 누락 경로와 위치 찾기/목록에서 제거 안내 확인. 목록에서 제거 후 다음 항목 표시. 시험 파일 이름은 원복. 위치 찾기 전체 연결 과정은 별도 추가 검증 가능. | [1](../tc-screenshots/fixes-20260923/28-missing-recent-guidance.png) [2](../tc-screenshots/fixes-20260923/29-missing-recent-removed.png) |
| TC-507 | 실패 → 부분 확인 | 전체화면의 Escape 복귀 결함 해결. 정지 프레임과 영상 위젯이 같은 편집 영역으로 복귀함. 실제 음량 변화 청감 검증은 남아 있어 TC 전체 통과로 처리하지 않음. | [1](../tc-screenshots/fixes-20260923/06-fullscreen.png) [2](../tc-screenshots/fixes-20260923/07-fullscreen-return.png) [3](../tc-screenshots/fixes-20260923/42-still-fullscreen.png) [4](../tc-screenshots/fixes-20260923/43-still-fullscreen-return.png) |
| TC-513 | 실패 → 통과 | 분할 영상 0.980~2.957초 선택 재생 끝에서 정지, 원본2.958초 프레임 표시. 새 녹화 파일도 0~2초 재생 후2.000초 정지, 선택 해제 시 양끝0초/음영 제거와 기존 전체19.633초/1조각/재생위치2초 유지. | [1](../tc-screenshots/fixes-20260923/37-selected-range-start.png) [2](../tc-screenshots/fixes-20260923/38-selected-range-end.png) [3](../tc-screenshots/fixes-20260923/56-selection-playing.png) [4](../tc-screenshots/fixes-20260923/57-selection-stopped.png) [5](../tc-screenshots/fixes-20260923/58-selection-cleared.png) |
| TC-515 | 실패 → 통과 | 0.980~2.957초 삭제: 8초/3조각 → 6.023초/4조각. 실행 취소 8초/3조각/2.957초 복원, 다시 실행 6.023초/4조각/0.980초 복원. 각 상태의 원본 프레임 2.958초 표시 확인. | [1](../tc-screenshots/fixes-20260923/39-delete-frame.png) [2](../tc-screenshots/fixes-20260923/40-delete-undo-frame.png) [3](../tc-screenshots/fixes-20260923/41-delete-redo-frame.png) |
| TC-521 | 실패 → 부분 확인 | 3초 AVI + 분할 MP4(총11초)를 2.420초부터 재생하여 기존 6.979초 경계 멈춤 없이 11.000초 끝에 도달, 원본 마지막 7.958초 프레임 확인. 음성 경계 청감/무음 간격 정밀 측정은 남음. | [1](../tc-screenshots/fixes-20260923/34-before-mixed-boundary.png) [2](../tc-screenshots/fixes-20260923/35-boundary-play.png) [3](../tc-screenshots/fixes-20260923/36-boundary-progress.png) |
| TC-523 | 실패 → 통과 | 3초 AVI 앞 삽입 전 5.993초. 실행 취소로 8초/3조각/5.993초와 원본 6.000초 프레임 복원. 다시 실행으로 11초/4조각/5.993초 및 원본 MP4 3.000초 프레임 복원(앞 AVI 3초 반영). | [1](../tc-screenshots/fixes-20260923/31-merge-frame.png) [2](../tc-screenshots/fixes-20260923/32-merge-undo-restored.png) [3](../tc-screenshots/fixes-20260923/33-merge-redo-restored.png) |

### 내부 탐색기 추가 확인

- [녹화 폴더 목록](../tc-screenshots/fixes-20260923/51-recording-folder-files-1.png)에서 녹화 파일 더블클릭 → [편집 반영](../tc-screenshots/fixes-20260923/53-recording-loaded.png).
- [작업 이름 입력](../tc-screenshots/fixes-20260923/54-internal-save-1.png) → [저장 완료](../tc-screenshots/fixes-20260923/55-internal-saved.png). 생성된 `build_vs2022/test-recordings/fix-browser-save.edutool.json`의 원본 경로·19.633초·영상 정보를 확인했다.
- 앱 재실행 뒤 내부 작업 열기로 저장한 JSON을 열고 [1280×720 재생/레이아웃 유지](../tc-screenshots/fixes-20260923/61-layout-after-play.png) 확인.
- 새 탐색기는 편집/녹화 파일 흐름에 적용했다. OBS 소스 속성의 모든 외부 플러그인 파일 선택기를 대체한 것은 아니다. 원본 위치 찾기의 끝까지 연결 및 경로 변경·저장 실패의 모든 경계 조건은 추가 확인 대상이다.

### 확인 자료

- [97개 전체 통합 판정표](edutool-tc-results-20260923-after-fixes.md) / [JSON](edutool-tc-results-20260923-after-fixes.json)
- [참고 24장과 실제 UI 병렬 비교](../tc-screenshots/fixes-20260923/comparison.html)
- [수정 전 결과](edutool-tc-results-20260923.md)는 보존한다.

## 참고 이미지별 이번 작업 대응

| 번호 | 참고 화면 | 이번 범위 | 관찰/한계 |
|---|---|---|---|
| [01](../tc-screenshots/fixes-20260923/comparison.html#ref-01) | 녹화 폴더 | 수정/부분 재확인 | 앱 내부 녹화 폴더 뷰어에서 경로·여유 공간·실제 목록 표시. 녹화 파일 더블클릭 후 편집 화면 19.633초/1조각으로 반영. 폴더 변경/저용량 조건은 이번 미재시험. |
| [02](../tc-screenshots/fixes-20260923/comparison.html#ref-02) | 메인 공통 고급설정 | 이번 제외 | 사용자 요청으로 메인 공통 설정 제외. |
| [03](../tc-screenshots/fixes-20260923/comparison.html#ref-03) | 녹화 설정 | 이번 미재시험 | 이 항목은 이번에 다시 실행하지 않았으며 이전 검증 결과를 유지한다. |
| [04](../tc-screenshots/fixes-20260923/comparison.html#ref-04) | 소스 관리 | 이번 미재시험 | 이 항목은 이번에 다시 실행하지 않았으며 이전 검증 결과를 유지한다. |
| [05](../tc-screenshots/fixes-20260923/comparison.html#ref-05) | 업로드 설정 | 이번 제외 | 사용자 요청으로 로그인·서버 관련 동작 제외. 이전 자료는 이력으로만 보존. |
| [06](../tc-screenshots/fixes-20260923/comparison.html#ref-06) | 오디오 필터 | 이번 미재시험 | 이 항목은 이번에 다시 실행하지 않았으며 이전 검증 결과를 유지한다. |
| [07](../tc-screenshots/fixes-20260923/comparison.html#ref-07) | 오디오 설정 | 이번 미재시험 | 이 항목은 이번에 다시 실행하지 않았으며 이전 검증 결과를 유지한다. |
| [08](../tc-screenshots/fixes-20260923/comparison.html#ref-08) | 카메라 설정 | 이번 미재시험 | 이 항목은 이번에 다시 실행하지 않았으며 이전 검증 결과를 유지한다. |
| [09](../tc-screenshots/fixes-20260923/comparison.html#ref-09) | 카메라 장치 세부설정 | 이번 미재시험 | 이 항목은 이번에 다시 실행하지 않았으며 이전 검증 결과를 유지한다. |
| [10](../tc-screenshots/fixes-20260923/comparison.html#ref-10) | 녹화 시작 | 이번 미재시험 | 이 항목은 이번에 다시 실행하지 않았으며 이전 검증 결과를 유지한다. |
| [11](../tc-screenshots/fixes-20260923/comparison.html#ref-11) | 녹화 종료 | 수정/부분 재확인 | 추가 TC-127: 녹화 중 앱 종료 확인 1회 후 정상 종료. 생성 MP4 19.633333초, 전체 디코드 성공. 종료 후 서버 업로드는 제외. |
| [12](../tc-screenshots/fixes-20260923/comparison.html#ref-12) | 우측 패널 접기 | 이번 미재시험 | 이 항목은 이번에 다시 실행하지 않았으며 이전 검증 결과를 유지한다. |
| [13](../tc-screenshots/fixes-20260923/comparison.html#ref-13) | 녹화 메인 | 수정/부분 재확인 | 재시작과 기능 전환 후 녹화 제어판 유지. 편집 도크 겹침 해결. 참고 레이아웃과 완전한 픽셀 일치를 뜻하지 않음. |
| [14](../tc-screenshots/fixes-20260923/comparison.html#ref-14) | 라이브 메인 | 수정/부분 재확인 | 기능 전환 시 라이브 화면 표시만 확인. 로그인·실제 서버 연결·방송은 이번 제외. |
| [15](../tc-screenshots/fixes-20260923/comparison.html#ref-15) | 라이브 기능별 설정 | 이번 제외 | 사용자 요청으로 로그인·서버 관련 동작 제외. 이전 자료는 이력으로만 보존. |
| [16](../tc-screenshots/fixes-20260923/comparison.html#ref-16) | 로그인 후 계정 정보 | 이번 제외 | 사용자 요청으로 로그인·서버 관련 동작 제외. 이전 자료는 이력으로만 보존. |
| [17](../tc-screenshots/fixes-20260923/comparison.html#ref-17) | 상단 로그인 | 이번 제외 | 사용자 요청으로 로그인·서버 관련 동작 제외. 이전 자료는 이력으로만 보존. |
| [18](../tc-screenshots/fixes-20260923/comparison.html#ref-18) | 편집 구간 선택 | 수정/부분 재확인 | 선택 구간 끝에서 정지 및 선택 해제 확인. 삭제/실행 취소/다시 실행 시 길이·조각 수·프레임 복원. TC-513/515 통과. |
| [19](../tc-screenshots/fixes-20260923/comparison.html#ref-19) | 편집 영상 나누기 | 수정/부분 재확인 | 분할 작업 로드 후 5.993초 위치에서 원본 6.000초 프레임 표시. 이름/순서/분할의 전체 회귀 시험은 이번 미실시. |
| [20](../tc-screenshots/fixes-20260923/comparison.html#ref-20) | 편집 영상 합치기 | 수정/부분 재확인 | AVI 3초 앞 삽입 후 총11초. 실행 취소/다시 실행 시5.993초 보존과 해당 원본 프레임 표시. 경계 멈춤 없이 끝까지 재생. 음성 경계 청감은 남음. |
| [21](../tc-screenshots/fixes-20260923/comparison.html#ref-21) | 편집 확대·축소·전체 보기 | 수정/부분 재확인 | 시간 눈금 간격 보완. 화면 캡처로 현재 표시 확인. 확대/축소/전체 보기 전체 절차는 이전 결과 유지. |
| [22](../tc-screenshots/fixes-20260923/comparison.html#ref-22) | 편집 빈 화면 | 수정/부분 재확인 | 빈 편집 화면에서 도크 겹침 제거, 버튼 여러 행 배치. 참고의 최근 썸네일 카드 등 모든 디자인 일치가 완료된 것은 아님. |
| [23](../tc-screenshots/fixes-20260923/comparison.html#ref-23) | 영상 불러오기 창 | 수정/부분 재확인 | 프로젝트 내부 탐색기 신설. 형식 필터·경로·다중 선택·더블클릭·같은 창 드래그 지원. 실제 가져오기/드래그/작업 저장과 다시 열기 확인. 누락 최근 파일 위치 찾기/목록 제거 안내와 목록 제거 확인. 카드형 썸네일 UI는 구현하지 않음. |
| [24](../tc-screenshots/fixes-20260923/comparison.html#ref-24) | 영상 불러오기 완료 | 수정/부분 재확인 | 전체화면 종료 시 편집 영역 복귀, 메타데이터 줄바꿈. 1280×720 재생 후에도 편집 버튼과 미리보기 배치 유지. 최소 창 크기와 실소리 동기는 추가 확인 필요. |

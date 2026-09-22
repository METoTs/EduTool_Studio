# EduTool 홈페이지 어댑터 계약 (배포 서버 연결 필요)

이 문서는 앱이 구현한 **제안 어댑터 규격**이다. 기존 홈페이지가 이 API를 제공한다고 확인된 것은 아니다. 실제 인증·강좌·업로드 서버는 이 계약에 맞는 어댑터를 제공하거나 앱의 경로/매핑을 변경해야 한다. 기본 서버 주소·계정·토큰은 포함하지 않았다.

## 공통

- 연결 설정의 HTTPS 기본 주소 뒤에 아래 경로를 붙인다. 개발용 localhost/127.0.0.1만 HTTP 허용.
- UTF-8 JSON 요청·응답. 토큰은 `Authorization: Bearer …`. 3xx 리디렉션은 따라가지 않는다.
- 성공은 2xx와 올바른 JSON 응답을 모두 요구한다. 401은 로컬 세션 만료, 403은 권한 오류. 인증 실패 시 원본을 삭제하지 않는다.
- 응답의 사용자 이름·폴더명은 일반 텍스트로 취급한다. HTML을 실행하지 않는다.
- 비밀번호는 저장하지 않는다. Windows 로그인 유지 토큰은 DPAPI로 암호화한다. 다른 OS에서는 영구 세션 저장을 사용하지 않는다.

## 인증·대상 목록

| 메서드 / 경로 | 입력 | 응답 |
|---|---|---|
| POST /v1/auth/login | `email`, `password` | `access_token`, `name`, `email` |
| GET /v1/me | Bearer | `name`, `email` |
| POST /v1/auth/logout | Bearer | 서버 세션 폐기. 오프라인이어도 로컬 토큰은 제거 |
| POST /v1/auth/browser | `provider`: google 또는 microsoft | HTTPS `authorization_url`, 비밀 `poll_token`, Unix 초 `expires_at` |
| POST /v1/auth/browser/poll | `poll_token` | 대기: `status: pending`; 완료: `access_token`, `name` |
| GET /v1/folders | Bearer | `folders: [{id, name}]` (강좌를 포함한 표시 이름) |
| POST /v1/folders | `name` | 생성된 `id`, `name` |

브라우저 OAuth의 redirect URI, CSRF state, PKCE, Google/Microsoft client ID 등은 서버 어댑터가 관리한다. 앱은 서버가 발급한 일회성 polling token으로만 결과를 받는다. 서버는 짧은 만료시간과 polling 제한을 적용해야 한다. 앱에서 취소하면 polling을 중단한다.

## 업로드

`POST /v1/uploads`: multipart/form-data.

- metadata JSON: `folder_id`, `title`, `sha256` (소문자 hex), `size` (바이트), `collision: reject`.
- file: 스트리밍 전송하는 원본 파일 바디.
- `Idempotency-Key`: 서버 주소·폴더·제목·내용 해시에서 계산. 서버는 동일 키 재전송을 같은 업로드로 처리해야 한다.
- 응답: `id`.
- 같은 제목의 다른 내용은 409로 거부한다. 클라이언트가 임의 덮어쓰기를 요구하지 않는다.

`GET /v1/uploads/{percent-encoded-id}`:

```json
{"status":"complete","verified":true,"sha256":"…","size":12345,"title":"영상 제목","folder_id":"대상 ID"}
```

서버는 전체 바이트의 영구 저장과 해시 검증이 끝나기 전에는 verified=true를 반환하면 안 된다. 클라이언트는 상태·검증 여부·크기·SHA-256·제목·대상 폴더가 모두 일치해야 완료로 표시한다. 처리 중이거나 응답이 불완전하면 로컬 원본을 보존하고 재시도를 안내한다.

## 정책

- 중단된 전송은 다음 실행에 자동 전송하지 않고 재시도 가능한 상태와 원본 경로를 남긴다. 재시도는 서버 멱등성 계약을 사용한다.
- 자동 업로드는 사용자가 설정한 경우 녹화 파일이 닫힌 뒤 수행한다. 다른 작업이 진행 중이면 원래 홈페이지·폴더와 파일 경로를 대기열에 저장한다. 완료 후 다음 녹화를 처리하며, 재시작 후에는 ‘대기 녹화 업로드’로 재개한다.
- 삭제 옵션은 기본 OFF. 검증된 업로드에 한해 로컬 원본을 다시 해시 비교한 뒤 삭제를 시도한다. 실패·취소·미검증에서는 삭제하지 않는다.
- 편집 원본 참조 파일을 삭제하면 작업을 다시 열 때 원본 재연결이 필요하다. 배포 운영에서는 원본 보존이 기본이다.
- 실제 서버/계정은 제공되지 않았으므로 인증·업로드 서버 결과와 OAuth는 미검증이다.

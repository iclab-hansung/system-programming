# system-programming-example

CPU 멀티스레드로 이미지 추론을 돌리는 예제 프로그램

스레드마다 모델을 하나씩 들고, 공유 큐에서 작업을 꺼내가는 방식으로 동작

## 사용한 라이브러리/데이터

- [darknet](https://github.com/pjreddie/darknet) — 추론 엔진 (`lib/darknet`)
- [imagenet-sample-images](https://github.com/EliSchwartz/imagenet-sample-images) — 정답 라벨 검증용 이미지 (`data/`)

## 빌드 & 실행

```
bash init.sh    # 의존성 설치 + 모델 weight/cfg 다운로드
bash build.sh   # infer.o 빌드
./infer.o config.yaml
```

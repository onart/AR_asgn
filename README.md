## 코드
- src/main.cpp : ar_util.cpp의 함수를 호출, 이후 cg_util의 GL 루프로 이동
- src/cg_util.cpp : OpenGL, VAO 초기화, 입력에 따른 흐름 처리
- src/ar_util.cpp : 캘리브레이션과 마커 포즈 감지를 진행
- src/adhoc.cpp : 상태 3, 4, 5의 동작을 수행 후 GL 렌더링
- module.h, oagle.h, oagle_simd.h, oaglem.h, Shader.h, Shader.cpp: 편의를 위해 미리 작성된 모듈을 사용

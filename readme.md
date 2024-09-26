## L6解码流程图
```mermaid
graph TD;
    A[input_qzssr(ssrctx_t *sc, unsigned char data)] --> B{判断卫星号};
    B -->|是| C[执行操作1];
    B -->|否| D[执行操作2];
    C --> E[结束];
    D --> E;

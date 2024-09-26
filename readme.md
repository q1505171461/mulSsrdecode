## L6解码流程图
```mermaid
graph TD;
    A[input_qzssr] --> A1{reed-solomon校验};
    A1 -->|是| B[判断卫星号获取相应ssrctx_t];
    A1 -->|否| A;
    B --> C[拼接buffer];
    C --> D{判断下一个码类型是否为4073避免丢包导致的数据异常}
    D -->|是| E[解码数据更新ssrctx_t];
    D -->|否| F[重置ssrctx_t解码buffer];
    E --等待下次调用--> A;
    F --等待下次调用--> A;

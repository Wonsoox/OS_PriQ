519 한원석
진행한 범위 : 
요구조건 대부분 달성:
    우선순위 큐 구현방향 : 연결리스트(스켈레톤 코드의 구현방향) -> 힙(재선택한 우선순위 큐용도 선택 자료형)

    qtype.h
    queue.cpp
    url.txt
    ->모두 작성완료

    세부사항 : qtype.h를 힙구조에 맞게 변경시켜서 적용해놨고, queue.cpp도 heapify_down/up을 enqueue/dequeue에 적용시켜 힙으로 우선순위 큐를 구현하였습니다. 각각 mutex도 qtype.h에서 사용되어지는 Queue타입에 넣어두고 enqueue/dequeue/range모두에 mutex를 적용시켜놓아서 thread-safe기능도 구현하였습니다.
    처음부터 힙으로 구현할 생각이었어서 n-계열 함수들은 따로 밑으로 빼놓았습니다.
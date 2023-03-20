#include "core.h"
#include "tokenizer.h"
#include "socket.h"
#include "intercom.h"
#include "except.h"

///////////////////////////////////////////////////////////////
// Program Entry
///////////////////////////////////////////////////////////////

INT32 main(INT32 Argc, PCHAR Argv[])
{
  INT32 status = 0;

  // Initialize winsock
  status = UmInitializeWsa();
  if (status == 0)
  {
    __try
    {
      // Resolve remote address and port
      PASOCKET remoteSocket = NULL;
      PADDRINFOA remoteAddress = NULL;
      status = UmGetAddrInfo(Argv[1], Argv[2], (ADDRESS_FAMILY)AF_INET, (UINT16)SOCK_STREAM, IPPROTO_TCP, &remoteAddress);
      if (status == 0)
      {
        // Read evaluate print line
        HANDLE stdIn = GetStdHandle(STD_INPUT_HANDLE);
        while (TRUE)
        {
          CHAR charBuffer[32] = { 0 };
          UINT32 charsRead = 0;
          if (ReadConsole(stdIn, charBuffer, sizeof(charBuffer), &charsRead, NULL) != 0)
          {
            UINT32 tokenCount = 0;
            LPSTR* tokens = UmTokenizeString(charBuffer, &tokenCount);;

            // Create socket
            status = UmCreateSocket(&remoteSocket, remoteAddress);
            if (status == 0)
            {
              // Connect remote socket
              status = UmConnect(remoteSocket);
              if (status == 0)
              {
                LOG("Connected to server\n");
            
                REQUEST_TYPE requestType = atoi(tokens[0]);
                UINT32 requestTypeSize = sizeof(requestType);
                status = UmSend(remoteSocket, &requestType, &requestTypeSize, 0);
            
                REQUEST_SCAN requestScan = { 0 };
                requestScan.Pid = atoi(tokens[1]);
                requestScan.NumberOfBytes = atoi(tokens[2]);
                UINT32 requestScanSize = sizeof(requestScan);
                status = UmSend(remoteSocket, &requestScan, &requestScanSize, 0);

                INT32 requestValue = atoi(tokens[3]);
                UINT32 requestValueSize = sizeof(requestValue);
                status = UmSend(remoteSocket, &requestValue, &requestValueSize, 0);

                //UINT32 recvBufferSize = sizeof(recvBuffer) - 1;
                //status = WsaRecv(Socket, recvBuffer, &recvBufferSize, 0);
            
                // Shutdown remote socket
                UmShutdownSocket(remoteSocket);
            
                LOG("Disconnected from server\n");
              }
            
              // Close remote socket
              UmCloseSocket(remoteSocket);
            }

            // Close socket
            UmFreeTokens(tokens, tokenCount);
          }
        }

        // Free remote address
        UmFreeAddrInfo(remoteAddress);
      }
    }
    __except (DEFAULT_EXCEPTION_HANDLER)
    {
      status = STATUS_UNHANDLED_EXCEPTION;
    }

    // Deinitialize winsock
    UmDeinitializeWsa();
  }

  return status;
}
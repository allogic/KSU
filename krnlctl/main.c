#include "core.h"
#include "tokenizer.h"
#include "socket.h"
#include "intercom.h"
#include "except.h"

INT32 main(INT32 Argc, PCHAR Argv[])
{
  INT32 status = 0;

  // Initialize winsock
  status = KcSocketInitialize();
  if (status == 0)
  {
    __try
    {
      // Resolve remote address and port
      PASOCKET remoteSocket = NULL;
      PADDRINFOA remoteAddress = NULL;
      status = KcGetAddrInfo(Argv[1], Argv[2], (ADDRESS_FAMILY)AF_INET, (UINT16)SOCK_STREAM, IPPROTO_TCP, &remoteAddress);
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
            LPSTR* tokens = KcTokenizeString(charBuffer, &tokenCount);;

            // Create socket
            status = KcCreateSocket(&remoteSocket, remoteAddress);
            if (status == 0)
            {
              // Connect remote socket
              status = KcConnect(remoteSocket);
              if (status == 0)
              {
                LOG("Connected to server\n");
            
                REQUEST_TYPE requestType = atoi(tokens[0]);
                UINT32 requestTypeSize = sizeof(requestType);
                status = KcSend(remoteSocket, &requestType, &requestTypeSize, 0);
            
                REQUEST_SCAN requestScan = { 0 };
                requestScan.Pid = atoi(tokens[1]);
                requestScan.NumberOfBytes = atoi(tokens[2]);
                UINT32 requestScanSize = sizeof(requestScan);
                status = KcSend(remoteSocket, &requestScan, &requestScanSize, 0);

                INT32 requestValue = atoi(tokens[3]);
                UINT32 requestValueSize = sizeof(requestValue);
                status = KcSend(remoteSocket, &requestValue, &requestValueSize, 0);

                //UINT32 recvBufferSize = sizeof(recvBuffer) - 1;
                //status = WsaRecv(Socket, recvBuffer, &recvBufferSize, 0);
            
                // Shutdown remote socket
                KcShutdownSocket(remoteSocket);
            
                LOG("Disconnected from server\n");
              }
            
              // Close remote socket
              KcCloseSocket(remoteSocket);
            }

            // Close socket
            KcFreeTokens(tokens, tokenCount);
          }
        }

        // Free remote address
        KcFreeAddrInfo(remoteAddress);
      }
    }
    __except (DEFAULT_EXCEPTION_HANDLER)
    {
      status = STATUS_UNHANDLED_EXCEPTION;
    }

    // Deinitialize winsock
    KcSocketDeinitialize();
  }

  return status;
}
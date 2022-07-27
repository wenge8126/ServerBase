using System.Threading.Tasks;
using Logic;


    public class ClientActor : Actor
    {
        public override async void Start()
        {
            LOG.log("Client start");

            CG_RequestPlayerData msg = new CG_RequestPlayerData();

            var playerData = await AsyncRequestMsg(new UnitID((int)EActorType.Actor_Player, GetID().id), msg, 16000);
            if (playerData!=null)
                playerData.dump("@@@@@@@@@@@");
            else
            {
                LOG.logError("Request player data fail");
            }
        }

        public static async Task<NiceData> On(ClientActor actor, GC_RequestClientState msg)
        {
            msg.mMsgData.dump("*********=======");
            CG_ResponseClientState s = new CG_ResponseClientState();
            s.mInfo = "CLIENT RESPONSE";
            s.mBeginSecond = 944444444;
            return s.mMsgData;
        }

        public override void RegisterMsg()
        {
            RegisterMsg<ClientActor, GC_RequestClientState>(ClientActor.On);
        }
    }

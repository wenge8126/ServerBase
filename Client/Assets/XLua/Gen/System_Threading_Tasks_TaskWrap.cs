#if USE_UNI_LUA
using LuaAPI = UniLua.Lua;
using RealStatePtr = UniLua.ILuaState;
using LuaCSFunction = UniLua.CSharpFunctionDelegate;
#else
using LuaAPI = XLua.LuaDLL.Lua;
using RealStatePtr = System.IntPtr;
using LuaCSFunction = XLua.LuaDLL.lua_CSFunction;
#endif

using XLua;
using System.Collections.Generic;


namespace XLua.CSObjectWrap
{
    using Utils = XLua.Utils;
    public class SystemThreadingTasksTaskWrap 
    {
        public static void __Register(RealStatePtr L)
        {
			ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			System.Type type = typeof(System.Threading.Tasks.Task);
			Utils.BeginObjectRegister(type, L, translator, 0, 7, 9, 0);
			
			Utils.RegisterFunc(L, Utils.METHOD_IDX, "Start", _m_Start);
			Utils.RegisterFunc(L, Utils.METHOD_IDX, "RunSynchronously", _m_RunSynchronously);
			Utils.RegisterFunc(L, Utils.METHOD_IDX, "Dispose", _m_Dispose);
			Utils.RegisterFunc(L, Utils.METHOD_IDX, "GetAwaiter", _m_GetAwaiter);
			Utils.RegisterFunc(L, Utils.METHOD_IDX, "ConfigureAwait", _m_ConfigureAwait);
			Utils.RegisterFunc(L, Utils.METHOD_IDX, "Wait", _m_Wait);
			Utils.RegisterFunc(L, Utils.METHOD_IDX, "ContinueWith", _m_ContinueWith);
			
			
			Utils.RegisterFunc(L, Utils.GETTER_IDX, "Id", _g_get_Id);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "Exception", _g_get_Exception);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "Status", _g_get_Status);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "IsCanceled", _g_get_IsCanceled);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "IsCompleted", _g_get_IsCompleted);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "IsCompletedSuccessfully", _g_get_IsCompletedSuccessfully);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "CreationOptions", _g_get_CreationOptions);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "AsyncState", _g_get_AsyncState);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "IsFaulted", _g_get_IsFaulted);
            
			
			
			Utils.EndObjectRegister(type, L, translator, null, null,
			    null, null, null);

		    Utils.BeginClassRegister(type, L, __CreateInstance, 10, 3, 0);
			Utils.RegisterFunc(L, Utils.CLS_IDX, "Yield", _m_Yield_xlua_st_);
            Utils.RegisterFunc(L, Utils.CLS_IDX, "WaitAll", _m_WaitAll_xlua_st_);
            Utils.RegisterFunc(L, Utils.CLS_IDX, "WaitAny", _m_WaitAny_xlua_st_);
            Utils.RegisterFunc(L, Utils.CLS_IDX, "FromException", _m_FromException_xlua_st_);
            Utils.RegisterFunc(L, Utils.CLS_IDX, "FromCanceled", _m_FromCanceled_xlua_st_);
            Utils.RegisterFunc(L, Utils.CLS_IDX, "Run", _m_Run_xlua_st_);
            Utils.RegisterFunc(L, Utils.CLS_IDX, "Delay", _m_Delay_xlua_st_);
            Utils.RegisterFunc(L, Utils.CLS_IDX, "WhenAll", _m_WhenAll_xlua_st_);
            Utils.RegisterFunc(L, Utils.CLS_IDX, "WhenAny", _m_WhenAny_xlua_st_);
            
			
            
			Utils.RegisterFunc(L, Utils.CLS_GETTER_IDX, "CurrentId", _g_get_CurrentId);
            Utils.RegisterFunc(L, Utils.CLS_GETTER_IDX, "Factory", _g_get_Factory);
            Utils.RegisterFunc(L, Utils.CLS_GETTER_IDX, "CompletedTask", _g_get_CompletedTask);
            
			
			
			Utils.EndClassRegister(type, L, translator);
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int __CreateInstance(RealStatePtr L)
        {
            
			try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
				if(LuaAPI.lua_gettop(L) == 2 && translator.Assignable<System.Action>(L, 2))
				{
					System.Action _action = translator.GetDelegate<System.Action>(L, 2);
					
					var gen_ret = new System.Threading.Tasks.Task(_action);
					translator.Push(L, gen_ret);
                    
					return 1;
				}
				if(LuaAPI.lua_gettop(L) == 3 && translator.Assignable<System.Action>(L, 2) && translator.Assignable<System.Threading.CancellationToken>(L, 3))
				{
					System.Action _action = translator.GetDelegate<System.Action>(L, 2);
					System.Threading.CancellationToken _cancellationToken;translator.Get(L, 3, out _cancellationToken);
					
					var gen_ret = new System.Threading.Tasks.Task(_action, _cancellationToken);
					translator.Push(L, gen_ret);
                    
					return 1;
				}
				if(LuaAPI.lua_gettop(L) == 3 && translator.Assignable<System.Action>(L, 2) && translator.Assignable<System.Threading.Tasks.TaskCreationOptions>(L, 3))
				{
					System.Action _action = translator.GetDelegate<System.Action>(L, 2);
					System.Threading.Tasks.TaskCreationOptions _creationOptions;translator.Get(L, 3, out _creationOptions);
					
					var gen_ret = new System.Threading.Tasks.Task(_action, _creationOptions);
					translator.Push(L, gen_ret);
                    
					return 1;
				}
				if(LuaAPI.lua_gettop(L) == 4 && translator.Assignable<System.Action>(L, 2) && translator.Assignable<System.Threading.CancellationToken>(L, 3) && translator.Assignable<System.Threading.Tasks.TaskCreationOptions>(L, 4))
				{
					System.Action _action = translator.GetDelegate<System.Action>(L, 2);
					System.Threading.CancellationToken _cancellationToken;translator.Get(L, 3, out _cancellationToken);
					System.Threading.Tasks.TaskCreationOptions _creationOptions;translator.Get(L, 4, out _creationOptions);
					
					var gen_ret = new System.Threading.Tasks.Task(_action, _cancellationToken, _creationOptions);
					translator.Push(L, gen_ret);
                    
					return 1;
				}
				if(LuaAPI.lua_gettop(L) == 3 && translator.Assignable<System.Action<object>>(L, 2) && translator.Assignable<object>(L, 3))
				{
					System.Action<object> _action = translator.GetDelegate<System.Action<object>>(L, 2);
					object _state = translator.GetObject(L, 3, typeof(object));
					
					var gen_ret = new System.Threading.Tasks.Task(_action, _state);
					translator.Push(L, gen_ret);
                    
					return 1;
				}
				if(LuaAPI.lua_gettop(L) == 4 && translator.Assignable<System.Action<object>>(L, 2) && translator.Assignable<object>(L, 3) && translator.Assignable<System.Threading.CancellationToken>(L, 4))
				{
					System.Action<object> _action = translator.GetDelegate<System.Action<object>>(L, 2);
					object _state = translator.GetObject(L, 3, typeof(object));
					System.Threading.CancellationToken _cancellationToken;translator.Get(L, 4, out _cancellationToken);
					
					var gen_ret = new System.Threading.Tasks.Task(_action, _state, _cancellationToken);
					translator.Push(L, gen_ret);
                    
					return 1;
				}
				if(LuaAPI.lua_gettop(L) == 4 && translator.Assignable<System.Action<object>>(L, 2) && translator.Assignable<object>(L, 3) && translator.Assignable<System.Threading.Tasks.TaskCreationOptions>(L, 4))
				{
					System.Action<object> _action = translator.GetDelegate<System.Action<object>>(L, 2);
					object _state = translator.GetObject(L, 3, typeof(object));
					System.Threading.Tasks.TaskCreationOptions _creationOptions;translator.Get(L, 4, out _creationOptions);
					
					var gen_ret = new System.Threading.Tasks.Task(_action, _state, _creationOptions);
					translator.Push(L, gen_ret);
                    
					return 1;
				}
				if(LuaAPI.lua_gettop(L) == 5 && translator.Assignable<System.Action<object>>(L, 2) && translator.Assignable<object>(L, 3) && translator.Assignable<System.Threading.CancellationToken>(L, 4) && translator.Assignable<System.Threading.Tasks.TaskCreationOptions>(L, 5))
				{
					System.Action<object> _action = translator.GetDelegate<System.Action<object>>(L, 2);
					object _state = translator.GetObject(L, 3, typeof(object));
					System.Threading.CancellationToken _cancellationToken;translator.Get(L, 4, out _cancellationToken);
					System.Threading.Tasks.TaskCreationOptions _creationOptions;translator.Get(L, 5, out _creationOptions);
					
					var gen_ret = new System.Threading.Tasks.Task(_action, _state, _cancellationToken, _creationOptions);
					translator.Push(L, gen_ret);
                    
					return 1;
				}
				
			}
			catch(System.Exception gen_e) {
				return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
			}
            return LuaAPI.luaL_error(L, "invalid arguments to System.Threading.Tasks.Task constructor!");
            
        }
        
		
        
		
        
        
        
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_Start(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
            
            
			    int gen_param_count = LuaAPI.lua_gettop(L);
            
                if(gen_param_count == 1) 
                {
                    
                    gen_to_be_invoked.Start(  );
                    
                    
                    
                    return 0;
                }
                if(gen_param_count == 2&& translator.Assignable<System.Threading.Tasks.TaskScheduler>(L, 2)) 
                {
                    System.Threading.Tasks.TaskScheduler _scheduler = (System.Threading.Tasks.TaskScheduler)translator.GetObject(L, 2, typeof(System.Threading.Tasks.TaskScheduler));
                    
                    gen_to_be_invoked.Start( _scheduler );
                    
                    
                    
                    return 0;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
            return LuaAPI.luaL_error(L, "invalid arguments to System.Threading.Tasks.Task.Start!");
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_RunSynchronously(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
            
            
			    int gen_param_count = LuaAPI.lua_gettop(L);
            
                if(gen_param_count == 1) 
                {
                    
                    gen_to_be_invoked.RunSynchronously(  );
                    
                    
                    
                    return 0;
                }
                if(gen_param_count == 2&& translator.Assignable<System.Threading.Tasks.TaskScheduler>(L, 2)) 
                {
                    System.Threading.Tasks.TaskScheduler _scheduler = (System.Threading.Tasks.TaskScheduler)translator.GetObject(L, 2, typeof(System.Threading.Tasks.TaskScheduler));
                    
                    gen_to_be_invoked.RunSynchronously( _scheduler );
                    
                    
                    
                    return 0;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
            return LuaAPI.luaL_error(L, "invalid arguments to System.Threading.Tasks.Task.RunSynchronously!");
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_Dispose(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
            
            
                
                {
                    
                    gen_to_be_invoked.Dispose(  );
                    
                    
                    
                    return 0;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_GetAwaiter(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
            
            
                
                {
                    
                        var gen_ret = gen_to_be_invoked.GetAwaiter(  );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_ConfigureAwait(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
            
            
                
                {
                    bool _continueOnCapturedContext = LuaAPI.lua_toboolean(L, 2);
                    
                        var gen_ret = gen_to_be_invoked.ConfigureAwait( _continueOnCapturedContext );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_Yield_xlua_st_(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
            
                
                {
                    
                        var gen_ret = System.Threading.Tasks.Task.Yield(  );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_Wait(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
            
            
			    int gen_param_count = LuaAPI.lua_gettop(L);
            
                if(gen_param_count == 1) 
                {
                    
                    gen_to_be_invoked.Wait(  );
                    
                    
                    
                    return 0;
                }
                if(gen_param_count == 2&& LuaTypes.LUA_TNUMBER == LuaAPI.lua_type(L, 2)) 
                {
                    int _millisecondsTimeout = LuaAPI.xlua_tointeger(L, 2);
                    
                        var gen_ret = gen_to_be_invoked.Wait( _millisecondsTimeout );
                        LuaAPI.lua_pushboolean(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 2&& translator.Assignable<System.TimeSpan>(L, 2)) 
                {
                    System.TimeSpan _timeout;translator.Get(L, 2, out _timeout);
                    
                        var gen_ret = gen_to_be_invoked.Wait( _timeout );
                        LuaAPI.lua_pushboolean(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 2&& translator.Assignable<System.Threading.CancellationToken>(L, 2)) 
                {
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 2, out _cancellationToken);
                    
                    gen_to_be_invoked.Wait( _cancellationToken );
                    
                    
                    
                    return 0;
                }
                if(gen_param_count == 3&& LuaTypes.LUA_TNUMBER == LuaAPI.lua_type(L, 2)&& translator.Assignable<System.Threading.CancellationToken>(L, 3)) 
                {
                    int _millisecondsTimeout = LuaAPI.xlua_tointeger(L, 2);
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 3, out _cancellationToken);
                    
                        var gen_ret = gen_to_be_invoked.Wait( _millisecondsTimeout, _cancellationToken );
                        LuaAPI.lua_pushboolean(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
            return LuaAPI.luaL_error(L, "invalid arguments to System.Threading.Tasks.Task.Wait!");
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_ContinueWith(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
            
            
			    int gen_param_count = LuaAPI.lua_gettop(L);
            
                if(gen_param_count == 2&& translator.Assignable<System.Action<System.Threading.Tasks.Task>>(L, 2)) 
                {
                    System.Action<System.Threading.Tasks.Task> _continuationAction = translator.GetDelegate<System.Action<System.Threading.Tasks.Task>>(L, 2);
                    
                        var gen_ret = gen_to_be_invoked.ContinueWith( _continuationAction );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 3&& translator.Assignable<System.Action<System.Threading.Tasks.Task>>(L, 2)&& translator.Assignable<System.Threading.CancellationToken>(L, 3)) 
                {
                    System.Action<System.Threading.Tasks.Task> _continuationAction = translator.GetDelegate<System.Action<System.Threading.Tasks.Task>>(L, 2);
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 3, out _cancellationToken);
                    
                        var gen_ret = gen_to_be_invoked.ContinueWith( _continuationAction, _cancellationToken );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 3&& translator.Assignable<System.Action<System.Threading.Tasks.Task>>(L, 2)&& translator.Assignable<System.Threading.Tasks.TaskScheduler>(L, 3)) 
                {
                    System.Action<System.Threading.Tasks.Task> _continuationAction = translator.GetDelegate<System.Action<System.Threading.Tasks.Task>>(L, 2);
                    System.Threading.Tasks.TaskScheduler _scheduler = (System.Threading.Tasks.TaskScheduler)translator.GetObject(L, 3, typeof(System.Threading.Tasks.TaskScheduler));
                    
                        var gen_ret = gen_to_be_invoked.ContinueWith( _continuationAction, _scheduler );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 3&& translator.Assignable<System.Action<System.Threading.Tasks.Task>>(L, 2)&& translator.Assignable<System.Threading.Tasks.TaskContinuationOptions>(L, 3)) 
                {
                    System.Action<System.Threading.Tasks.Task> _continuationAction = translator.GetDelegate<System.Action<System.Threading.Tasks.Task>>(L, 2);
                    System.Threading.Tasks.TaskContinuationOptions _continuationOptions;translator.Get(L, 3, out _continuationOptions);
                    
                        var gen_ret = gen_to_be_invoked.ContinueWith( _continuationAction, _continuationOptions );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 3&& translator.Assignable<System.Action<System.Threading.Tasks.Task, object>>(L, 2)&& translator.Assignable<object>(L, 3)) 
                {
                    System.Action<System.Threading.Tasks.Task, object> _continuationAction = translator.GetDelegate<System.Action<System.Threading.Tasks.Task, object>>(L, 2);
                    object _state = translator.GetObject(L, 3, typeof(object));
                    
                        var gen_ret = gen_to_be_invoked.ContinueWith( _continuationAction, _state );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 4&& translator.Assignable<System.Action<System.Threading.Tasks.Task, object>>(L, 2)&& translator.Assignable<object>(L, 3)&& translator.Assignable<System.Threading.CancellationToken>(L, 4)) 
                {
                    System.Action<System.Threading.Tasks.Task, object> _continuationAction = translator.GetDelegate<System.Action<System.Threading.Tasks.Task, object>>(L, 2);
                    object _state = translator.GetObject(L, 3, typeof(object));
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 4, out _cancellationToken);
                    
                        var gen_ret = gen_to_be_invoked.ContinueWith( _continuationAction, _state, _cancellationToken );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 4&& translator.Assignable<System.Action<System.Threading.Tasks.Task, object>>(L, 2)&& translator.Assignable<object>(L, 3)&& translator.Assignable<System.Threading.Tasks.TaskScheduler>(L, 4)) 
                {
                    System.Action<System.Threading.Tasks.Task, object> _continuationAction = translator.GetDelegate<System.Action<System.Threading.Tasks.Task, object>>(L, 2);
                    object _state = translator.GetObject(L, 3, typeof(object));
                    System.Threading.Tasks.TaskScheduler _scheduler = (System.Threading.Tasks.TaskScheduler)translator.GetObject(L, 4, typeof(System.Threading.Tasks.TaskScheduler));
                    
                        var gen_ret = gen_to_be_invoked.ContinueWith( _continuationAction, _state, _scheduler );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 4&& translator.Assignable<System.Action<System.Threading.Tasks.Task, object>>(L, 2)&& translator.Assignable<object>(L, 3)&& translator.Assignable<System.Threading.Tasks.TaskContinuationOptions>(L, 4)) 
                {
                    System.Action<System.Threading.Tasks.Task, object> _continuationAction = translator.GetDelegate<System.Action<System.Threading.Tasks.Task, object>>(L, 2);
                    object _state = translator.GetObject(L, 3, typeof(object));
                    System.Threading.Tasks.TaskContinuationOptions _continuationOptions;translator.Get(L, 4, out _continuationOptions);
                    
                        var gen_ret = gen_to_be_invoked.ContinueWith( _continuationAction, _state, _continuationOptions );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 5&& translator.Assignable<System.Action<System.Threading.Tasks.Task>>(L, 2)&& translator.Assignable<System.Threading.CancellationToken>(L, 3)&& translator.Assignable<System.Threading.Tasks.TaskContinuationOptions>(L, 4)&& translator.Assignable<System.Threading.Tasks.TaskScheduler>(L, 5)) 
                {
                    System.Action<System.Threading.Tasks.Task> _continuationAction = translator.GetDelegate<System.Action<System.Threading.Tasks.Task>>(L, 2);
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 3, out _cancellationToken);
                    System.Threading.Tasks.TaskContinuationOptions _continuationOptions;translator.Get(L, 4, out _continuationOptions);
                    System.Threading.Tasks.TaskScheduler _scheduler = (System.Threading.Tasks.TaskScheduler)translator.GetObject(L, 5, typeof(System.Threading.Tasks.TaskScheduler));
                    
                        var gen_ret = gen_to_be_invoked.ContinueWith( _continuationAction, _cancellationToken, _continuationOptions, _scheduler );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 6&& translator.Assignable<System.Action<System.Threading.Tasks.Task, object>>(L, 2)&& translator.Assignable<object>(L, 3)&& translator.Assignable<System.Threading.CancellationToken>(L, 4)&& translator.Assignable<System.Threading.Tasks.TaskContinuationOptions>(L, 5)&& translator.Assignable<System.Threading.Tasks.TaskScheduler>(L, 6)) 
                {
                    System.Action<System.Threading.Tasks.Task, object> _continuationAction = translator.GetDelegate<System.Action<System.Threading.Tasks.Task, object>>(L, 2);
                    object _state = translator.GetObject(L, 3, typeof(object));
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 4, out _cancellationToken);
                    System.Threading.Tasks.TaskContinuationOptions _continuationOptions;translator.Get(L, 5, out _continuationOptions);
                    System.Threading.Tasks.TaskScheduler _scheduler = (System.Threading.Tasks.TaskScheduler)translator.GetObject(L, 6, typeof(System.Threading.Tasks.TaskScheduler));
                    
                        var gen_ret = gen_to_be_invoked.ContinueWith( _continuationAction, _state, _cancellationToken, _continuationOptions, _scheduler );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
            return LuaAPI.luaL_error(L, "invalid arguments to System.Threading.Tasks.Task.ContinueWith!");
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_WaitAll_xlua_st_(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
            
			    int gen_param_count = LuaAPI.lua_gettop(L);
            
                if(gen_param_count >= 0&& (LuaTypes.LUA_TNONE == LuaAPI.lua_type(L, 1) || translator.Assignable<System.Threading.Tasks.Task>(L, 1))) 
                {
                    System.Threading.Tasks.Task[] _tasks = translator.GetParams<System.Threading.Tasks.Task>(L, 1);
                    
                    System.Threading.Tasks.Task.WaitAll( _tasks );
                    
                    
                    
                    return 0;
                }
                if(gen_param_count == 2&& translator.Assignable<System.Threading.Tasks.Task[]>(L, 1)&& LuaTypes.LUA_TNUMBER == LuaAPI.lua_type(L, 2)) 
                {
                    System.Threading.Tasks.Task[] _tasks = (System.Threading.Tasks.Task[])translator.GetObject(L, 1, typeof(System.Threading.Tasks.Task[]));
                    int _millisecondsTimeout = LuaAPI.xlua_tointeger(L, 2);
                    
                        var gen_ret = System.Threading.Tasks.Task.WaitAll( _tasks, _millisecondsTimeout );
                        LuaAPI.lua_pushboolean(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 2&& translator.Assignable<System.Threading.Tasks.Task[]>(L, 1)&& translator.Assignable<System.TimeSpan>(L, 2)) 
                {
                    System.Threading.Tasks.Task[] _tasks = (System.Threading.Tasks.Task[])translator.GetObject(L, 1, typeof(System.Threading.Tasks.Task[]));
                    System.TimeSpan _timeout;translator.Get(L, 2, out _timeout);
                    
                        var gen_ret = System.Threading.Tasks.Task.WaitAll( _tasks, _timeout );
                        LuaAPI.lua_pushboolean(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 2&& translator.Assignable<System.Threading.Tasks.Task[]>(L, 1)&& translator.Assignable<System.Threading.CancellationToken>(L, 2)) 
                {
                    System.Threading.Tasks.Task[] _tasks = (System.Threading.Tasks.Task[])translator.GetObject(L, 1, typeof(System.Threading.Tasks.Task[]));
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 2, out _cancellationToken);
                    
                    System.Threading.Tasks.Task.WaitAll( _tasks, _cancellationToken );
                    
                    
                    
                    return 0;
                }
                if(gen_param_count == 3&& translator.Assignable<System.Threading.Tasks.Task[]>(L, 1)&& LuaTypes.LUA_TNUMBER == LuaAPI.lua_type(L, 2)&& translator.Assignable<System.Threading.CancellationToken>(L, 3)) 
                {
                    System.Threading.Tasks.Task[] _tasks = (System.Threading.Tasks.Task[])translator.GetObject(L, 1, typeof(System.Threading.Tasks.Task[]));
                    int _millisecondsTimeout = LuaAPI.xlua_tointeger(L, 2);
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 3, out _cancellationToken);
                    
                        var gen_ret = System.Threading.Tasks.Task.WaitAll( _tasks, _millisecondsTimeout, _cancellationToken );
                        LuaAPI.lua_pushboolean(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
            return LuaAPI.luaL_error(L, "invalid arguments to System.Threading.Tasks.Task.WaitAll!");
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_WaitAny_xlua_st_(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
            
			    int gen_param_count = LuaAPI.lua_gettop(L);
            
                if(gen_param_count >= 0&& (LuaTypes.LUA_TNONE == LuaAPI.lua_type(L, 1) || translator.Assignable<System.Threading.Tasks.Task>(L, 1))) 
                {
                    System.Threading.Tasks.Task[] _tasks = translator.GetParams<System.Threading.Tasks.Task>(L, 1);
                    
                        var gen_ret = System.Threading.Tasks.Task.WaitAny( _tasks );
                        LuaAPI.xlua_pushinteger(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 2&& translator.Assignable<System.Threading.Tasks.Task[]>(L, 1)&& LuaTypes.LUA_TNUMBER == LuaAPI.lua_type(L, 2)) 
                {
                    System.Threading.Tasks.Task[] _tasks = (System.Threading.Tasks.Task[])translator.GetObject(L, 1, typeof(System.Threading.Tasks.Task[]));
                    int _millisecondsTimeout = LuaAPI.xlua_tointeger(L, 2);
                    
                        var gen_ret = System.Threading.Tasks.Task.WaitAny( _tasks, _millisecondsTimeout );
                        LuaAPI.xlua_pushinteger(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 2&& translator.Assignable<System.Threading.Tasks.Task[]>(L, 1)&& translator.Assignable<System.TimeSpan>(L, 2)) 
                {
                    System.Threading.Tasks.Task[] _tasks = (System.Threading.Tasks.Task[])translator.GetObject(L, 1, typeof(System.Threading.Tasks.Task[]));
                    System.TimeSpan _timeout;translator.Get(L, 2, out _timeout);
                    
                        var gen_ret = System.Threading.Tasks.Task.WaitAny( _tasks, _timeout );
                        LuaAPI.xlua_pushinteger(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 2&& translator.Assignable<System.Threading.Tasks.Task[]>(L, 1)&& translator.Assignable<System.Threading.CancellationToken>(L, 2)) 
                {
                    System.Threading.Tasks.Task[] _tasks = (System.Threading.Tasks.Task[])translator.GetObject(L, 1, typeof(System.Threading.Tasks.Task[]));
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 2, out _cancellationToken);
                    
                        var gen_ret = System.Threading.Tasks.Task.WaitAny( _tasks, _cancellationToken );
                        LuaAPI.xlua_pushinteger(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 3&& translator.Assignable<System.Threading.Tasks.Task[]>(L, 1)&& LuaTypes.LUA_TNUMBER == LuaAPI.lua_type(L, 2)&& translator.Assignable<System.Threading.CancellationToken>(L, 3)) 
                {
                    System.Threading.Tasks.Task[] _tasks = (System.Threading.Tasks.Task[])translator.GetObject(L, 1, typeof(System.Threading.Tasks.Task[]));
                    int _millisecondsTimeout = LuaAPI.xlua_tointeger(L, 2);
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 3, out _cancellationToken);
                    
                        var gen_ret = System.Threading.Tasks.Task.WaitAny( _tasks, _millisecondsTimeout, _cancellationToken );
                        LuaAPI.xlua_pushinteger(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
            return LuaAPI.luaL_error(L, "invalid arguments to System.Threading.Tasks.Task.WaitAny!");
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_FromException_xlua_st_(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
            
                
                {
                    System.Exception _exception = (System.Exception)translator.GetObject(L, 1, typeof(System.Exception));
                    
                        var gen_ret = System.Threading.Tasks.Task.FromException( _exception );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_FromCanceled_xlua_st_(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
            
                
                {
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 1, out _cancellationToken);
                    
                        var gen_ret = System.Threading.Tasks.Task.FromCanceled( _cancellationToken );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_Run_xlua_st_(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
            
			    int gen_param_count = LuaAPI.lua_gettop(L);
            
                if(gen_param_count == 1&& translator.Assignable<System.Action>(L, 1)) 
                {
                    System.Action _action = translator.GetDelegate<System.Action>(L, 1);
                    
                        var gen_ret = System.Threading.Tasks.Task.Run( _action );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 1&& translator.Assignable<System.Func<System.Threading.Tasks.Task>>(L, 1)) 
                {
                    System.Func<System.Threading.Tasks.Task> _function = translator.GetDelegate<System.Func<System.Threading.Tasks.Task>>(L, 1);
                    
                        var gen_ret = System.Threading.Tasks.Task.Run( _function );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 2&& translator.Assignable<System.Action>(L, 1)&& translator.Assignable<System.Threading.CancellationToken>(L, 2)) 
                {
                    System.Action _action = translator.GetDelegate<System.Action>(L, 1);
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 2, out _cancellationToken);
                    
                        var gen_ret = System.Threading.Tasks.Task.Run( _action, _cancellationToken );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 2&& translator.Assignable<System.Func<System.Threading.Tasks.Task>>(L, 1)&& translator.Assignable<System.Threading.CancellationToken>(L, 2)) 
                {
                    System.Func<System.Threading.Tasks.Task> _function = translator.GetDelegate<System.Func<System.Threading.Tasks.Task>>(L, 1);
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 2, out _cancellationToken);
                    
                        var gen_ret = System.Threading.Tasks.Task.Run( _function, _cancellationToken );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
            return LuaAPI.luaL_error(L, "invalid arguments to System.Threading.Tasks.Task.Run!");
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_Delay_xlua_st_(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
            
			    int gen_param_count = LuaAPI.lua_gettop(L);
            
                if(gen_param_count == 1&& LuaTypes.LUA_TNUMBER == LuaAPI.lua_type(L, 1)) 
                {
                    int _millisecondsDelay = LuaAPI.xlua_tointeger(L, 1);
                    
                        var gen_ret = System.Threading.Tasks.Task.Delay( _millisecondsDelay );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 1&& translator.Assignable<System.TimeSpan>(L, 1)) 
                {
                    System.TimeSpan _delay;translator.Get(L, 1, out _delay);
                    
                        var gen_ret = System.Threading.Tasks.Task.Delay( _delay );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 2&& LuaTypes.LUA_TNUMBER == LuaAPI.lua_type(L, 1)&& translator.Assignable<System.Threading.CancellationToken>(L, 2)) 
                {
                    int _millisecondsDelay = LuaAPI.xlua_tointeger(L, 1);
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 2, out _cancellationToken);
                    
                        var gen_ret = System.Threading.Tasks.Task.Delay( _millisecondsDelay, _cancellationToken );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 2&& translator.Assignable<System.TimeSpan>(L, 1)&& translator.Assignable<System.Threading.CancellationToken>(L, 2)) 
                {
                    System.TimeSpan _delay;translator.Get(L, 1, out _delay);
                    System.Threading.CancellationToken _cancellationToken;translator.Get(L, 2, out _cancellationToken);
                    
                        var gen_ret = System.Threading.Tasks.Task.Delay( _delay, _cancellationToken );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
            return LuaAPI.luaL_error(L, "invalid arguments to System.Threading.Tasks.Task.Delay!");
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_WhenAll_xlua_st_(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
            
			    int gen_param_count = LuaAPI.lua_gettop(L);
            
                if(gen_param_count == 1&& translator.Assignable<System.Collections.Generic.IEnumerable<System.Threading.Tasks.Task>>(L, 1)) 
                {
                    System.Collections.Generic.IEnumerable<System.Threading.Tasks.Task> _tasks = (System.Collections.Generic.IEnumerable<System.Threading.Tasks.Task>)translator.GetObject(L, 1, typeof(System.Collections.Generic.IEnumerable<System.Threading.Tasks.Task>));
                    
                        var gen_ret = System.Threading.Tasks.Task.WhenAll( _tasks );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count >= 0&& (LuaTypes.LUA_TNONE == LuaAPI.lua_type(L, 1) || translator.Assignable<System.Threading.Tasks.Task>(L, 1))) 
                {
                    System.Threading.Tasks.Task[] _tasks = translator.GetParams<System.Threading.Tasks.Task>(L, 1);
                    
                        var gen_ret = System.Threading.Tasks.Task.WhenAll( _tasks );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
            return LuaAPI.luaL_error(L, "invalid arguments to System.Threading.Tasks.Task.WhenAll!");
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_WhenAny_xlua_st_(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
            
			    int gen_param_count = LuaAPI.lua_gettop(L);
            
                if(gen_param_count >= 0&& (LuaTypes.LUA_TNONE == LuaAPI.lua_type(L, 1) || translator.Assignable<System.Threading.Tasks.Task>(L, 1))) 
                {
                    System.Threading.Tasks.Task[] _tasks = translator.GetParams<System.Threading.Tasks.Task>(L, 1);
                    
                        var gen_ret = System.Threading.Tasks.Task.WhenAny( _tasks );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                if(gen_param_count == 1&& translator.Assignable<System.Collections.Generic.IEnumerable<System.Threading.Tasks.Task>>(L, 1)) 
                {
                    System.Collections.Generic.IEnumerable<System.Threading.Tasks.Task> _tasks = (System.Collections.Generic.IEnumerable<System.Threading.Tasks.Task>)translator.GetObject(L, 1, typeof(System.Collections.Generic.IEnumerable<System.Threading.Tasks.Task>));
                    
                        var gen_ret = System.Threading.Tasks.Task.WhenAny( _tasks );
                        translator.Push(L, gen_ret);
                    
                    
                    
                    return 1;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
            return LuaAPI.luaL_error(L, "invalid arguments to System.Threading.Tasks.Task.WhenAny!");
            
        }
        
        
        
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_Id(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
                LuaAPI.xlua_pushinteger(L, gen_to_be_invoked.Id);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_CurrentId(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			    translator.PushAny(L, System.Threading.Tasks.Task.CurrentId);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_Exception(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
                translator.Push(L, gen_to_be_invoked.Exception);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_Status(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
                translator.Push(L, gen_to_be_invoked.Status);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_IsCanceled(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
                LuaAPI.lua_pushboolean(L, gen_to_be_invoked.IsCanceled);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_IsCompleted(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
                LuaAPI.lua_pushboolean(L, gen_to_be_invoked.IsCompleted);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_IsCompletedSuccessfully(RealStatePtr L)
        {
		    // try {
      //           ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			   //
      //           System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
      //           LuaAPI.lua_pushboolean(L, gen_to_be_invoked.IsCompletedSuccessfully);
      //       } catch(System.Exception gen_e) {
      //           return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
      //       }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_CreationOptions(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
                translator.Push(L, gen_to_be_invoked.CreationOptions);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_AsyncState(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
                translator.PushAny(L, gen_to_be_invoked.AsyncState);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_Factory(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			    translator.Push(L, System.Threading.Tasks.Task.Factory);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_CompletedTask(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			    translator.Push(L, System.Threading.Tasks.Task.CompletedTask);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_IsFaulted(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                System.Threading.Tasks.Task gen_to_be_invoked = (System.Threading.Tasks.Task)translator.FastGetCSObj(L, 1);
                LuaAPI.lua_pushboolean(L, gen_to_be_invoked.IsFaulted);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        
        
		
		
		
		
    }
}

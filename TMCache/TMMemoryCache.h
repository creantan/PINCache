/**
 `TMMemoryCache` is a fast, thread safe key/value store similar to `NSCache`. On iOS it will clear itself
 automatically to reduce memory usage when the app receives a warning or moves into the background.

 Access is natively asynchronous. Every method accepts a callback block that runs on a concurrent
 <queue>, with cache writes protected by GCD barriers. Synchronous variations are provided, but take
 care not to call them within the asynchronous callback blocks so as not the deadlock the queue.
 
 All access to the cache is dated so the that the least-used objects can be trimmed first. Setting an
 optional <ageLimit> will trigger a GCD timer to periodically to trim the cache to that age.
 
 Values will not persist after application relaunch or returning from the background. See <TMCache> for
 a memory cache backed by a disk cache.
 */

@class TMMemoryCache;

typedef void (^TMMemoryCacheBlock)(TMMemoryCache *cache);
typedef void (^TMMemoryCacheObjectBlock)(TMMemoryCache *cache, NSString *key, id object);

@interface TMMemoryCache : NSObject

#pragma mark -
/// @name Core

/**
 A concurrent queue on which all work is done. It is exposed here so that it can be set to target some
 other queue, such as a global concurrent queue with a priority other than the default.
 */
@property (readonly) dispatch_queue_t queue;

/**
 The maximum number of seconds an object is allowed to exist in the cache. Setting this to a value
 greater than `0.0` will start a recurring GCD timer with the same period that calls <trimToDate:>.
 Setting it back to `0.0` will stop the timer. Defaults to `0.0`.
 */
@property (assign) NSTimeInterval ageLimit;

#pragma mark -
/// @name Event Blocks

/**
 A block to be executed just before an object is added to the cache. This block will be excuted within
 a barrier, i.e. all reads and writes are suspended for the duration of the block.
 */
@property (copy) TMMemoryCacheObjectBlock willAddObjectBlock;

/**
 A block to be executed just before an object is removed from the cache. This block will be excuted
 within a barrier, i.e. all reads and writes are suspended for the duration of the block.
 */
@property (copy) TMMemoryCacheObjectBlock willRemoveObjectBlock;

/**
 A block to be executed just after an object is added to the cache. This block will be excuted within
 a barrier, i.e. all reads and writes are suspended for the duration of the block.
 */
@property (copy) TMMemoryCacheObjectBlock didAddObjectBlock;

/**
 A block to be executed just after an object is removed from the cache. This block will be excuted
 within a barrier, i.e. all reads and writes are suspended for the duration of the block.
 */
@property (copy) TMMemoryCacheObjectBlock didRemoveObjectBlock;

#pragma mark -
/// @name Shared Cache

/**
 A shared cache.
 
 @return The shared singleton cache instance.
 */
+ (instancetype)sharedCache;

#pragma mark -
/// @name Asynchronous Methods

/**
 Retrieves the object for the specified key. This method returns immediately and executes the passed
 block as soon as the object is available, potentially in parallel with other blocks on the <queue>.
 
 @param key The key associated with the requested object.
 @param block A block to be executed concurrently when the object is available.
 */
- (void)objectForKey:(NSString *)key block:(TMMemoryCacheObjectBlock)block;

/**
 Stores an object in the cache for the specified key. This method returns immediately and executes the
 passed block after the object has been stored, potentially in parallel with other blocks on the <queue>.
 
 @param object An object to store in the cache.
 @param key A key to associate with the object. This string will be copied.
 @param block A block to be executed concurrently after the object has been stored, or nil.
 */
- (void)setObject:(id)object forKey:(NSString *)key block:(TMMemoryCacheObjectBlock)block;

/**
 Removes the object for the specified key. This method returns immediately and executes the passed
 block after the object has been removed, potentially in parallel with other blocks on the <queue>.
 
 @param key The key associated with the object to be removed.
 @param block A block to be executed concurrently after the object has been removed, or nil.
 */
- (void)removeObjectForKey:(NSString *)key block:(TMMemoryCacheObjectBlock)block;

/**
 Removes all objects from the cache older than the specified date, as ordered by access time. This
 method returns immediately and executes the passed block after the cache has been trimmed, potentially
 in parallel with other blocks on the <queue>.
 
 @param date Objects that haven't been accessed since this date are removed from the cache.
 @param block A block to be executed concurrently after the cache has been trimmed, or nil.
 */
- (void)trimToDate:(NSDate *)date block:(TMMemoryCacheBlock)block;

/**
 Removes all objects from the cache without calling the associated event blocks. This method returns
 immediately and executes the passed block after the cache has been cleared, potentially in parallel
 with other blocks on the <queue>.
 
 @param block A block to be executed concurrently after the cache has been cleared, or nil.
 */
- (void)removeAllObjects:(TMMemoryCacheBlock)block;

#pragma mark -
/// @name Synchronous Methods

/**
 Retrieves the object for the specified key. This method blocks the calling the thread until the
 object is available.
 
 @see objectForKey:block:
 @param key The key associated with the object.
 @return The object for the specified key.
 */
- (id)objectForKey:(NSString *)key;

/**
 Stores an object in the cache for the specified key. This method blocks the calling the thread until
 the object has been stored.

 @see setObject:forKey:block:
 @param object An object to store in the cache.
 @param key A key to associate with the object. This string will be copied.
 */
- (void)setObject:(id)object forKey:(NSString *)key;

/**
 Removes the object for the specified key. This method blocks the calling the thread until the object
 has been removed.
 
 @param key The key associated with the object to be removed.
 */
- (void)removeObjectForKey:(NSString *)key;

/**
 Removes all objects from the cache older than the specified date, as ordered by access time. This
 method blocks the calling the thread until the cache has been trimmed.
 
 @param date Objects that haven't been accessed since this date are removed from the cache.
 */
- (void)trimToDate:(NSDate *)date;

/**
 Removes all objects from the cache without calling the associated event blocks. This method blocks
 the calling the thread until the cache has been cleared.
 */
- (void)removeAllObjects;

@end
